#ifndef TORRENT_ENGINE_H
#define TORRENT_ENGINE_H

#include <string>
#include <atomic>
#include <memory>

// EngineOptions used by main to configure engine
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

    // Add magnet and initialize; returns true on success
    bool start();

    // Block until finished (handles display updates itself)
    void loop();

    // Stop and cleanup early
    void stop();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

#endif // TORRENT_ENGINE_H
