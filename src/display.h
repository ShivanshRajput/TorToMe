#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>

struct SpeedSample {
    int down_kBps = 0;
    int up_kBps = 0;
    int peers = 0;
    long long done_bytes = 0;
    long long total_bytes = 0;
};

void show_progress_bar(const std::string& name, const SpeedSample& s, bool verbose);
std::string human_bytes(long long bytes);

#endif