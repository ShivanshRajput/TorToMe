#include "config.h"
#include <filesystem>
#include <sys/statvfs.h>
#include <iostream>

namespace fs = std::filesystem;

std::string get_download_path(const std::string& user_path, const std::string& default_path) {
    if (!user_path.empty()) {
        fs::create_directories(user_path);
        return user_path;
    }

    std::cout << "No path specified. Use default (" << default_path << ")? [y/n]: ";
    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "y" || choice == "Y" || choice.empty()) {
        fs::create_directories(default_path);
        return default_path;
    }

    std::string custom_path;
    std::cout << "Enter custom path: ";
    std::getline(std::cin, custom_path);
    if (custom_path.empty()) {
        fs::create_directories(default_path);
        return default_path;
    }

    fs::create_directories(custom_path);
    return custom_path;
}

bool check_space(const std::string& path, long long required_bytes) {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) {
        std::cerr << "Error: Cannot check disk space for " << path << "\n";
        return false;
    }
    long long free = stat.f_bsize * stat.f_bavail;
    std::cout << "Available space in " << path << ": " << free / (1024 * 1024) << " MB\n";
    if (free < required_bytes) {
        std::cerr << "Error: Not enough disk space in " << path << " (required: "
                  << required_bytes / (1024 * 1024) << " MB, available: "
                  << free / (1024 * 1024) << " MB)\n";
        return false;
    }
    return true;
}