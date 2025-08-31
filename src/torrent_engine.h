#ifndef TORRENT_ENGINE_H
#define TORRENT_ENGINE_H
#include <string>
class TorrentEngine {
public:
    TorrentEngine(int ulimit, int dlimit);
    void add_magnet(const std::string& magnet, const std::string& save_path);
    void run();
};
#endif