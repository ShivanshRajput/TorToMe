#include "display.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

std::string human_bytes(long long bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double val = static_cast<double>(bytes);
    int idx = 0;
    while (val >= 1024.0 && idx < 4) { val /= 1024.0; ++idx; }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision((idx == 0) ? 0 : 2) << val << ' ' << units[idx];
    return oss.str();
}

void show_progress_bar(const std::string& name, const SpeedSample& s, bool verbose) {
    int percent = 0;
    if (s.total_bytes > 0) {
        percent = static_cast<int>((100.0 * s.done_bytes) / std::max(1LL, s.total_bytes));
    }
    if (percent > 100) percent = 100;

    const int barWidth = 40;
    int pos = barWidth * percent / 100;

    std::ostringstream line;
    line << "\r" << name << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) line << '=';
        else if (i == pos) line << '>';
        else line << ' ';
    }
    line << "] " << std::setw(3) << percent << "%  "
         << human_bytes(s.done_bytes) << " / " << human_bytes(s.total_bytes)
         << "  ↓" << s.down_kBps << "kB/s ↑" << s.up_kBps << "kB/s";

    if (verbose) line << "  peers:" << s.peers;

    std::cout << line.str() << std::flush;

    if (percent >= 100) std::cout << '\n';
}