#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>

#include "config.h"
#include "torrent_engine.h"

namespace po = boost::program_options;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    // Options
    po::options_description desc("TorToMe options");
    desc.add_options()
        ("help,h", "Show help")
        ("url,u", po::value<std::string>()->value_name("MAGNET")->required(), "Magnet link")
        ("ulimit", po::value<int>()->default_value(0), "Upload limit (kB/s)")
        ("dlimit", po::value<int>()->default_value(0), "Download limit (kB/s)")
        ("path,p", po::value<std::string>()->default_value(""), "Download path")
        ("verbose,v", "Verbose (peer list)");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
            return 0;
        }

        po::notify(vm);
    } catch (const std::exception& e) {
        std::cerr << "Arg error: " << e.what() << "\n\n" << desc << '\n';
        return 1;
    }

    // Compute default path relative to build/run dir's parent
    std::string project_root;
    try {
        project_root = fs::current_path().parent_path().string();
    } catch (...) {
        project_root = ".";
    }
    std::string default_download_path = project_root + "/downloads";

    std::string user_path = vm["path"].as<std::string>();
    std::string download_path = get_download_path(user_path, default_download_path);
    std::cout << "Download path: " << download_path << '\n';

    if (!check_space(download_path, 0)) {
        return 1;
    }

    EngineOptions opts;
    opts.magnet_uri = vm["url"].as<std::string>();
    opts.save_path = download_path;
    opts.ulimit_kBps = vm["ulimit"].as<int>();
    opts.dlimit_kBps = vm["dlimit"].as<int>();
    opts.verbose = vm.count("verbose") > 0;

    TorrentEngine engine(opts);
    if (!engine.start()) return 1;
    engine.loop();
    return 0;
}