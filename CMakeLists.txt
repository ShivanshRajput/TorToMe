cmake_minimum_required(VERSION 3.12)
project(TorToMe CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boost (program_options)
find_package(Boost REQUIRED COMPONENTS program_options)

# libtorrent-rasterbar via pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBTORRENT REQUIRED libtorrent-rasterbar)

# include src and ui directories so headers can be found
include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/ui ${LIBTORRENT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_directories(${LIBTORRENT_LIBRARY_DIRS})

add_executable(torrent_cli
    src/main.cpp
    src/config.cpp
    src/torrent_engine.cpp
    src/ui.cpp
    ui/display.cpp
    ui/progress_bar.cpp
    ui/peer_table.cpp
)

target_link_libraries(torrent_cli
    PRIVATE
        ${Boost_LIBRARIES}
        ${LIBTORRENT_LIBRARIES}
)

# Threads
find_package(Threads REQUIRED)
target_link_libraries(torrent_cli PRIVATE Threads::Threads)
