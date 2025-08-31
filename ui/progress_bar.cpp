#include "progress_bar.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace ui {

static const char* RESET = "\033[0m";
static const char* GREEN = "\033[32m";
static const char* YELLOW = "\033[33m";
static const char* CYAN = "\033[36m";
static const char* BOLD = "\033[1m";

void renderProgressBar(const std::string &name,
                       double progress_percent,
                       double downloaded_MB,
                       double total_MB,
                       double down_kBps,
                       double up_kBps) {
    const int barWidth = 40;
    int pos = static_cast<int>(progress_percent * barWidth / 100.0);
    if (pos < 0) pos = 0;
    if (pos > barWidth) pos = barWidth;

    std::ostringstream oss;
    oss << BOLD << CYAN << name << RESET << "\n";
    oss << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) oss << GREEN << '=' << RESET;
        else if (i == pos) oss << '>';
        else oss << ' ';
    }
    oss << "] " << std::setw(6) << std::fixed << std::setprecision(1) << progress_percent << "%  ";
    oss << std::setw(7) << std::right << downloaded_MB << " MB / " << std::setw(6) << total_MB << " MB  ";
    oss << GREEN << "↓" << static_cast<int>(down_kBps) << "kB/s" << RESET << " ";
    oss << YELLOW << "↑" << static_cast<int>(up_kBps) << "kB/s" << RESET << "\n";

    std::cout << oss.str();
}

} // namespace ui
