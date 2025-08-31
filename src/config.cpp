#include "config.h"
#include <filesystem>
#include <iostream>
#include <sys/statvfs.h>

namespace fs = std::filesystem;

std::string get_download_path(const std::string& user_path, const std::string& default_path) {
    if (!user_path.empty()) {
        fs::create_directories(user_path);
        return user_path;
    }

    std::cout << "Enter download path (default: " << default_path << "): ";
    std::string choice;
    std::getline(std::cin, choice);

    std::string path = choice.empty() ? default_path : choice;
    fs::create_directories(path);
    return path;
}

bool check_space(const std::string& path, long long required_bytes) {
    struct statvfs stat{};
    if (statvfs(path.c_str(), &stat) != 0) {
        std::cerr << "Could not check free space!\n";
        return false;
    }
    unsigned long long free = static_cast<unsigned long long>(stat.f_bsize) * stat.f_bavail;
    std::cout << "Available space in " << path << ": " << (free / (1024 * 1024)) << " MB\n";
    if (required_bytes > 0 && free < static_cast<unsigned long long>(required_bytes)) {
        std::cerr << "Not enough space. Required: " << (required_bytes / (1024 * 1024))
                  << " MB, Available: " << (free / (1024 * 1024)) << " MB\n";
        return false;
    }
    return true;
}
