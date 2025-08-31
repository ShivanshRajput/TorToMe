#ifndef DISPLAY_H
#define DISPLAY_H
#include <string>  // Added for std::string
void show_progress(float progress, long long downloaded, const std::string& file_name, bool verbose);
#endif