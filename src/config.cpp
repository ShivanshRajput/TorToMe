#include "config.h"
#include <filesystem>
#include <sys/statvfs.h>
#include <iostream>
namespace fs = std::filesystem;
std::string get_download_path(const std::string& user_path) {
    return user_path.empty() ? fs::current_path().string() + "/downloads" : user_path;
}
bool check_space(const std::string& path, long long required_bytes) {
    struct statvfs stat;
    if (statvfs(path.c_str(), &stat) != 0) return false;
    return (stat.f_bsize * stat.f_bavail) >= required_bytes;
}