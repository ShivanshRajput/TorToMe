#ifndef CONFIG_H
#define CONFIG_H

#include <string>

std::string get_download_path(const std::string& user_path, const std::string& default_path);
bool check_space(const std::string& path, long long required_bytes);

#endif // CONFIG_H
