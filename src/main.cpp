#include <boost/program_options.hpp>
#include <iostream>
#include "config.h"
#include <filesystem>

namespace po = boost::program_options;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Show help")
        ("url,u", po::value<std::string>(), "Magnet link")
        ("ulimit", po::value<int>()->default_value(0), "Upload limit (KB/s)")
        ("dlimit", po::value<int>()->default_value(0), "Download limit (KB/s)")
        ("path,p", po::value<std::string>(), "Download path");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << "Error parsing arguments: " << e.what() << "\n";
        std::cout << desc << "\n";
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    // Compute project root dynamically using current_path (build/), parent is root
    std::string project_root = fs::current_path().parent_path().string();
    std::string default_download_path = project_root + "/downloads";

    std::string user_path = vm.count("path") ? vm["path"].as<std::string>() : "";
    std::string download_path = get_download_path(user_path, default_download_path);
    std::cout << "Download path set to: " << download_path << "\n";

    // Check space (1MB for testing)
    if (!check_space(download_path, 1'000'000)) {
        return 1;
    }

    std::cout << "Torrent CLI starting...\n";
    return 0;
}