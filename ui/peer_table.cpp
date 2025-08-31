#include "peer_table.hpp"
#include <iostream>
#include <iomanip>

namespace ui {

void renderPeerTable(const std::vector<PeerInfo> &peers) {
    std::cout << "\n" << "\033[1mActive Peers:\033[0m\n";
    std::cout << std::left << std::setw(22) << "IP"
              << std::setw(8) << "Port"
              << std::setw(10) << "Progress"
              << std::setw(12) << "Down(kB/s)"
              << std::setw(12) << "Up(kB/s)"
              << std::setw(8) << "Choked"
              << std::setw(10) << "Interested" << "\n";
    std::cout << std::string(82, '-') << "\n";

    int shown = 0;
    for (const auto& p : peers) {
        if (shown++ >= 20) break;
        std::cout << std::left << std::setw(22) << p.ip
                  << std::setw(8) << p.port
                  << std::setw(9) << std::fixed << std::setprecision(1) << p.progress << "%"
                  << std::setw(12) << p.down_kBps
                  << std::setw(12) << p.up_kBps
                  << std::setw(8) << (p.choked ? "Yes" : "No")
                  << std::setw(10) << (p.interested ? "Yes" : "No")
                  << "\n";
    }
    std::cout << std::endl;
}

} // namespace ui
