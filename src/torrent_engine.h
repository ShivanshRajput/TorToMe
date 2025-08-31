#ifndef TORRENT_ENGINE_H
#define TORRENT_ENGINE_H

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/alert.hpp>

struct EngineOptions {
    int ulimit_kBps = 0;
    int dlimit_kBps = 0;
    bool verbose = false;
    std::string save_path;
    std::string magnet_uri;
};

class TorrentEngine {
public:
    explicit TorrentEngine(const EngineOptions& opts);
    ~TorrentEngine();

    bool start();  // add magnet, start session
    void loop();   // main loop until finished or interrupted
    void stop();

private:
    EngineOptions opts_;
    std::unique_ptr<libtorrent::session> ses_;
    libtorrent::torrent_handle th_;
    std::atomic<bool> running_{true};

    void apply_rate_limits();
    bool add_magnet();
    void handle_alerts(std::vector<libtorrent::alert*>& alerts);
};

#endif