#include "display.h"
#include <iostream>
void show_progress(float progress, long long downloaded, const std::string& file_name, bool verbose) {
    std::cout << "\rProgress: " << progress * 100 << "%\n";
}