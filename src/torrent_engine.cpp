#include "torrent_engine.h"
#include <iostream>
TorrentEngine::TorrentEngine(int ulimit, int dlimit) { std::cout << "Engine init\n"; }
void TorrentEngine::add_magnet(const std::string& magnet, const std::string& save_path) {}
void TorrentEngine::run() {}