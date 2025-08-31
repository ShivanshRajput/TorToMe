#ifndef UI_DISPLAY_HPP
#define UI_DISPLAY_HPP

#include "progress_bar.hpp"
#include "peer_table.hpp"
#include <string>
#include <vector>
#include <chrono>

namespace ui {

// Speed sample used for summary printing
struct SpeedSample {
    int down_kBps = 0;
    int up_kBps = 0;
    int peers = 0;
    long long done_bytes = 0;
    long long total_bytes = 0;
};

// lifecycle
void init();
void clear();
void shutdown();

// displays
void showDownloadStatus(const std::string& name,
                        double progress_percent,
                        double downloaded_MB,
                        double total_MB,
                        double down_kBps,
                        double up_kBps);

void showPeers(const std::vector<PeerInfo>& peers);

// print final summary
void printSummary(const std::string& name, const SpeedSample& s, std::chrono::steady_clock::time_point start);

} // namespace ui

#endif // UI_DISPLAY_HPP
