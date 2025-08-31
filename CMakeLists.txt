cmake_minimum_required(VERSION 3.12)
project(TorToMe CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boost (program_options)
find_package(Boost REQUIRED COMPONENTS program_options)

# libtorrent-rasterbar via pkg-config (portable across many distros)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBTORRENT REQUIRED libtorrent-rasterbar)

add_executable(torrent_cli
    src/main.cpp
    src/config.cpp
    src/display.cpp
    src/torrent_engine.cpp
)

target_include_directories(torrent_cli
    PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${LIBTORRENT_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/src
)

target_link_directories(torrent_cli PRIVATE ${LIBTORRENT_LIBRARY_DIRS})

target_link_libraries(torrent_cli
    PRIVATE
        ${Boost_LIBRARIES}
        ${LIBTORRENT_LIBRARIES}
)

# Prefer -pthread if needed
find_package(Threads REQUIRED)
target_link_libraries(torrent_cli PRIVATE Threads::Threads)
