#include "torrent_engine.h"
#include "display.h"

#include <libtorrent/alert_types.hpp>
#include <libtorrent/settings_pack.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/peer_info.hpp>

#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

namespace lt = libtorrent;

static std::atomic<bool> g_sigint{false};
static void sigint_handler(int) { g_sigint = true; }

TorrentEngine::TorrentEngine(const EngineOptions& opts) : opts_(opts) {
    lt::settings_pack sp;
    // Listen on default ports; you can customize if needed
    sp.set_int(lt::settings_pack::alert_mask,
               lt::alert::status_notification |
               lt::alert::error_notification  |
               lt::alert::storage_notification |
               lt::alert::tracker_notification);
    if (opts_.dlimit_kBps > 0) sp.set_int(lt::settings_pack::download_rate_limit, opts_.dlimit_kBps * 1024);
    if (opts_.ulimit_kBps > 0) sp.set_int(lt::settings_pack::upload_rate_limit, opts_.ulimit_kBps * 1024);

    ses_ = std::make_unique<lt::session>(sp);
}

TorrentEngine::~TorrentEngine() { stop(); }

void TorrentEngine::apply_rate_limits() {
    if (!ses_) return;
    lt::settings_pack sp;
    sp.set_int(lt::settings_pack::download_rate_limit,
               opts_.dlimit_kBps > 0 ? opts_.dlimit_kBps * 1024 : 0);
    sp.set_int(lt::settings_pack::upload_rate_limit,
               opts_.ulimit_kBps > 0 ? opts_.ulimit_kBps * 1024 : 0);
    ses_->apply_settings(sp);
}

bool TorrentEngine::add_magnet() {
    lt::error_code ec;
    lt::add_torrent_params params = lt::parse_magnet_uri(opts_.magnet_uri, ec);
    if (ec) {
        std::cerr << "Failed to parse magnet: " << ec.message() << '\n';
        return false;
    }
    params.save_path = opts_.save_path;
    params.flags |= lt::torrent_flags::seed_mode; // removed later if metadata fetched; helps fast start
    th_ = ses_->add_torrent(params, ec);
    if (ec) {
        std::cerr << "add_torrent error: " << ec.message() << '\n';
        return false;
    }
    return true;
}

bool TorrentEngine::start() {
    std::signal(SIGINT, sigint_handler);
    std::signal(SIGTERM, sigint_handler);

    apply_rate_limits();
    if (!add_magnet()) return false;

    std::cout << "Added torrent. Saving to: " << opts_.save_path << '\n';
    return true;
}

void TorrentEngine::handle_alerts(std::vector<lt::alert*>& alerts) {
    for (lt::alert* a : alerts) {
        if (auto* at = lt::alert_cast<lt::add_torrent_alert>(a)) {
            auto name = at->handle.is_valid() && at->handle.torrent_file()
                        ? at->handle.torrent_file()->name()
                        : std::string("(fetching metadata)");
            std::cout << "Added: " << name << '\n';
        }
        if (auto* fin = lt::alert_cast<lt::torrent_finished_alert>(a)) {
            std::cout << "\nFinished: " << fin->handle.status().name << '\n';
            running_ = false;
        }
        if (auto* err = lt::alert_cast<lt::torrent_error_alert>(a)) {
            std::cerr << "\nError: " << err->error.message() << '\n';
            running_ = false;
        }
    }
}

void TorrentEngine::loop() {
    using namespace std::chrono_literals;

    while (running_ && !g_sigint) {
        // pump alerts
        std::vector<lt::alert*> alerts;
        ses_->pop_alerts(&alerts);
        if (!alerts.empty()) handle_alerts(alerts);

        // ask for status updates once per tick
        ses_->post_torrent_updates();

        if (th_.is_valid()) {
            lt::torrent_status s = th_.status();
            SpeedSample smp;
            smp.down_kBps = s.download_rate / 1000;
            smp.up_kBps = s.upload_rate / 1000;
            smp.done_bytes = s.total_wanted_done;
            smp.total_bytes = s.total_wanted;
            smp.peers = s.num_peers;

            std::string name = s.name;
            if (name.empty() && th_.torrent_file()) name = th_.torrent_file()->name();

            show_progress_bar(name.empty() ? "(unknown)" : name, smp, opts_.verbose);

            // optional: detailed peer list when verbose, print once every few seconds
            static int tick = 0;
            if (opts_.verbose && (++tick % 5 == 0)) {
                std::vector<lt::peer_info> peers;
                th_.get_peer_info(peers);
                std::cout << "\nPeers (" << peers.size() << "):";
                for (auto const& p : peers) {
                    std::cout << " " << p.ip.address().to_string()
                              << " ↓" << p.down_speed / 1000 << "kB/s"
                              << " ↑" << p.up_speed / 1000 << "kB/s";
                }
                std::cout << '\n';
            }

            if (s.is_finished) {
                std::cout << "\nAll files completed.\n";
                running_ = false;
            }
        }

        std::this_thread::sleep_for(1s);
    }
    std::cout << "\nExiting...\n";
}

void TorrentEngine::stop() {
    running_ = false;
    if (ses_) {
        // Graceful: let session destructor handle cleanup
        ses_.reset();
    }
}