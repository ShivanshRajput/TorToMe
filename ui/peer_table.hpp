#ifndef UI_PEER_TABLE_HPP
#define UI_PEER_TABLE_HPP

#include <string>
#include <vector>

namespace ui {

struct PeerInfo {
    std::string ip;
    int port = 0;
    float progress = 0.0f;   // percent 0..100
    int down_kBps = 0;
    int up_kBps = 0;
    bool choked = false;
    bool interested = false;
};

void renderPeerTable(const std::vector<PeerInfo> &peers);

} // namespace ui

#endif // UI_PEER_TABLE_HPP
