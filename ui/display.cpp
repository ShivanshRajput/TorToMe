#include "display.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

namespace ui {

void init() {
    // clear screen and print header
    std::cout << "\033[2J\033[1;1H";
    std::cout << "\033[36m==================== TorToMe Torrent Client ====================\033[0m\n";
}

void clear() {
    // Move cursor to home and clear to end — less flicker than full clear on some terminals
    std::cout << "\033[H\033[J";
    std::cout << "\033[36m==================== TorToMe Torrent Client ====================\033[0m\n";
}

void shutdown() {
    // nothing fancy for now
}

void showDownloadStatus(const std::string& name,
                        double progress_percent,
                        double downloaded_MB,
                        double total_MB,
                        double down_kBps,
                        double up_kBps) {
    // delegate to progress_bar renderer
    renderProgressBar(name, progress_percent, downloaded_MB, total_MB, down_kBps, up_kBps);
}

void showPeers(const std::vector<PeerInfo>& peers) {
    if (peers.empty()) {
        // show a compact peers-info line when there are no peers or verbose is off
        std::cout << "\nPeers: " << 0 << " (use --verbose to see peer table)\n\n";
        return;
    }
    renderPeerTable(peers);
}

void printSummary(const std::string& name, const SpeedSample& s, std::chrono::steady_clock::time_point start) {
    auto end = std::chrono::steady_clock::now();
    auto elapsed_s = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "\n\033[36m=== Download Summary ===\033[0m\n";
    std::cout << "File: " << name << "\n";
    std::cout << "Size: " << std::fixed << std::setprecision(2) << (double)s.total_bytes / (1024.0*1024.0) << " MB\n";
    std::cout << "Downloaded: " << std::fixed << std::setprecision(2) << (double)s.done_bytes / (1024.0*1024.0) << " MB\n";
    std::cout << "Time: " << elapsed_s << " s\n";
    if (elapsed_s > 0) {
        double avg = (double)s.done_bytes / (double)elapsed_s;
        std::cout << "Average speed: " << std::fixed << std::setprecision(2) << avg / 1024.0 << " kB/s\n";
    }
    std::cout << "\033[32m=========================\033[0m\n";
}

} // namespace ui
