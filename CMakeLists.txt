cmake_minimum_required(VERSION 3.10)
project(TorrentCLI)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find pkg-config
find_package(PkgConfig REQUIRED)

# Find libtorrent-rasterbar using pkg-config
pkg_check_modules(LIBTORRENT REQUIRED libtorrent-rasterbar)

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options filesystem)

# Add executable
add_executable(torrent-cli src/main.cpp src/config.cpp src/torrent_engine.cpp src/display.cpp)

# Link libraries
target_include_directories(torrent-cli PRIVATE ${LIBTORRENT_INCLUDE_DIRS})
target_link_libraries(torrent-cli PRIVATE ${LIBTORRENT_LIBRARIES} Boost::program_options Boost::filesystem pthread)