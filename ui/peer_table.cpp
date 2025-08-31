#include "peer_table.hpp"
#include <iostream>
#include <iomanip>

namespace ui {

void renderPeerTable(const std::vector<PeerInfo> &peers) {
    std::cout << "\n" << "\033[1mActive Peers:\033[0m\n";

    // headers
    std::cout << std::left << std::setw(42) << "IP"   // IPv6 safe
              << std::setw(8)  << "Port"
              << std::setw(12) << "Progress"
              << std::setw(12) << "Down(kB/s)"
              << std::setw(12) << "Up(kB/s)"
              << std::setw(10) << "Choked"
              << std::setw(12) << "Interested"
              << "\n";

    std::cout << std::string(108, '-') << "\n";  // adjusted separator

    int shown = 0;
    for (const auto& p : peers) {
        if (shown++ >= 20) break;

        std::ostringstream prog;
        prog << std::fixed << std::setprecision(0) << p.progress << "%";

        std::cout << std::left << std::setw(42) << p.ip
                  << std::setw(8)  << p.port
                  << std::setw(12) << prog.str()
                  << std::setw(12) << p.down_kBps
                  << std::setw(12) << p.up_kBps
                  << std::setw(10) << (p.choked ? "Yes" : "No")
                  << std::setw(12) << (p.interested ? "Yes" : "No")
                  << "\n";
    }

    std::cout << std::endl;
}

} // namespace ui
