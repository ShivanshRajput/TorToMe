#ifndef UI_PROGRESS_BAR_HPP
#define UI_PROGRESS_BAR_HPP

#include <string>

namespace ui {
    // Render a single-line progress bar with stats.
    void renderProgressBar(const std::string &name,
                           double progress_percent,     // 0..100
                           double downloaded_MB,
                           double total_MB,
                           double down_kBps,
                           double up_kBps);
}

#endif // UI_PROGRESS_BAR_HPP
