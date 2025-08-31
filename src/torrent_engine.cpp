#include "torrent_engine.h"
#include "config.h"
#include "ui.h"                // includes ../ui/display.hpp
#include <libtorrent/session.hpp>
#include <libtorrent/settings_pack.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/peer_info.hpp>

#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>
#include <vector>

namespace lt = libtorrent;

static std::atomic<bool> g_sigint{false};
static void sigint_handler(int) { g_sigint = true; }

struct TorrentEngine::Impl {
    EngineOptions opts;
    std::unique_ptr<lt::session> ses;
    lt::torrent_handle th;
    std::atomic<bool> running{true};

    Impl(const EngineOptions& o) : opts(o) {
        lt::settings_pack sp;
        sp.set_int(lt::settings_pack::alert_mask,
               lt::alert::status_notification |
               lt::alert::error_notification |
               lt::alert::storage_notification |
               lt::alert::tracker_notification);
        if (opts.dlimit_kBps > 0) sp.set_int(lt::settings_pack::download_rate_limit, opts.dlimit_kBps * 1024);
        if (opts.ulimit_kBps > 0) sp.set_int(lt::settings_pack::upload_rate_limit, opts.ulimit_kBps * 1024);

        ses = std::make_unique<lt::session>(sp);
    }

    ~Impl() {
        if (ses) ses.reset();
    }

    void apply_rate_limits() {
        if (!ses) return;
        lt::settings_pack sp;
        sp.set_int(lt::settings_pack::download_rate_limit,
                   opts.dlimit_kBps > 0 ? opts.dlimit_kBps * 1024 : 0);
        sp.set_int(lt::settings_pack::upload_rate_limit,
                   opts.ulimit_kBps > 0 ? opts.ulimit_kBps * 1024 : 0);
        ses->apply_settings(sp);
    }

    bool add_magnet() {
        lt::error_code ec;
        lt::add_torrent_params params = lt::parse_magnet_uri(opts.magnet_uri, ec);
        if (ec) {
            std::cerr << "Failed to parse magnet: " << ec.message() << '\n';
            return false;
        }
        params.save_path = opts.save_path;
        th = ses->add_torrent(params, ec);
        if (ec) {
            std::cerr << "add_torrent error: " << ec.message() << '\n';
            return false;
        }
        return true;
    }

    void handle_alerts(std::vector<lt::alert*>& alerts) {
        for (lt::alert* a : alerts) {
            if (auto* at = lt::alert_cast<lt::add_torrent_alert>(a)) {
                auto name = at->handle.is_valid() && at->handle.torrent_file()
                            ? at->handle.torrent_file()->name()
                            : std::string("(fetching metadata)");
                std::cout << "Added: " << name << '\n';
            }
            if (auto* fin = lt::alert_cast<lt::torrent_finished_alert>(a)) {
                std::cout << "\nFinished: " << fin->handle.status().name << '\n';
                running = false;
            }
            if (auto* err = lt::alert_cast<lt::torrent_error_alert>(a)) {
                std::cerr << "\nError: " << err->error.message() << '\n';
                running = false;
            }
        }
    }
};

TorrentEngine::TorrentEngine(const EngineOptions& opts) : impl_(std::make_unique<Impl>(opts)) {
    std::signal(SIGINT, sigint_handler);
    std::signal(SIGTERM, sigint_handler);
}

TorrentEngine::~TorrentEngine() {
    stop();
}

bool TorrentEngine::start() {
    if (!impl_) return false;
    impl_->apply_rate_limits();
    if (!impl_->add_magnet()) return false;
    std::cout << "Added torrent. Saving to: " << impl_->opts.save_path << '\n';
    return true;
}

void TorrentEngine::loop() {
    using namespace std::chrono_literals;
    auto start_time = std::chrono::steady_clock::now();

    // initialize UI once
    ui::init();

    while (impl_->running && !g_sigint) {
        // 1) pump alerts
        std::vector<lt::alert*> alerts;
        impl_->ses->pop_alerts(&alerts);
        if (!alerts.empty()) impl_->handle_alerts(alerts);

        // 2) request status updates
        impl_->ses->post_torrent_updates();

        // 3) clear & render fresh frame so previous output is overwritten
        ui::clear();

        if (impl_->th.is_valid()) {
            lt::torrent_status s = impl_->th.status();

            ui::SpeedSample smp;
            smp.down_kBps = static_cast<int>(s.download_rate / 1000);
            smp.up_kBps = static_cast<int>(s.upload_rate / 1000);
            smp.done_bytes = s.total_wanted_done;
            smp.total_bytes = s.total_wanted;
            smp.peers = s.num_peers;

            std::string name = s.name;
            if (name.empty() && impl_->th.torrent_file()) name = impl_->th.torrent_file()->name();

            // Show progress (redraw replaces previous lines)
            ui::showDownloadStatus(name.empty() ? "(unknown)" : name,
                                   (s.total_wanted > 0 ? (100.0 * s.total_wanted_done / s.total_wanted) : 0.0),
                                   static_cast<double>(s.total_wanted_done) / (1024.0*1024.0),
                                   static_cast<double>(s.total_wanted) / (1024.0*1024.0),
                                   static_cast<double>(s.download_rate) / 1024.0,
                                   static_cast<double>(s.upload_rate) / 1024.0);

            // If verbose, collect peer info and show table (live)
            if (impl_->opts.verbose) {
                std::vector<lt::peer_info> peers;
                impl_->th.get_peer_info(peers);
                std::vector<ui::PeerInfo> out;
                out.reserve(peers.size());
                for (auto const& p : peers) {
                    ui::PeerInfo pi;
                    pi.ip = p.ip.address().to_string();
                    pi.port = p.ip.port();

                    // progress: try progress_ppm then fallback to progress
                    #if defined(PS_PROGRESS_PPM)
                    pi.progress = p.progress_ppm / 10000.0f;
                    #else
                    if constexpr (std::is_member_object_pointer<decltype(&lt::peer_info::progress)>::value) {
                        pi.progress = static_cast<float>(p.progress * 100.0f);
                    } else {
                        // some versions provide progress_ppm
                        pi.progress = p.progress_ppm / 10000.0f;
                    }
                    #endif

                    pi.down_kBps = static_cast<int>(p.down_speed / 1024);
                    pi.up_kBps = static_cast<int>(p.up_speed / 1024);
                    pi.choked = (p.flags & lt::peer_info::choked) != 0;
                    pi.interested = (p.flags & lt::peer_info::interesting) != 0;
                    out.push_back(pi);
                }
                ui::showPeers(out);
            } else {
                // show a concise "peers count" line so the UI doesn't look empty
                std::vector<ui::PeerInfo> empty; // empty list
                ui::showPeers(empty);
            }

            if (s.is_finished) {
                std::cout << "\nAll files completed.\n";
                ui::printSummary(name.empty() ? "(unknown)" : name, smp, start_time);
                impl_->running = false;
            }
        } else {
            // If torrent_handle not yet valid, show placeholder header and peer area
            ui::showDownloadStatus("(waiting for metadata)", 0.0, 0.0, 0.0, 0.0, 0.0);
            std::vector<ui::PeerInfo> empty;
            ui::showPeers(empty);
        }

        std::this_thread::sleep_for(1s);
    }

    ui::shutdown();
    std::cout << "\nExiting...\n";
}

void TorrentEngine::stop() {
    if (!impl_) return;
    impl_->running = false;
    if (impl_->ses) impl_->ses.reset();
}
