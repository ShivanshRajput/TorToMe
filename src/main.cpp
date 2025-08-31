#include <boost/program_options.hpp>
#include <iostream>
namespace po = boost::program_options;
int main(int argc, char* argv[]) {
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Show help")
        ("url,u", po::value<std::string>(), "Magnet link")
        ("ulimit", po::value<int>()->default_value(0), "Upload limit (KB/s)")
        ("dlimit", po::value<int>()->default_value(0), "Download limit (KB/s)")
        ("path,p", po::value<std::string>(), "Download path");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }
    std::cout << "Torrent CLI starting...\n";
    return 0;
}