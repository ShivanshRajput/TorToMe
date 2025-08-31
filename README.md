# TorToMe — CLI Torrent Client (C++ / libtorrent)

## Build (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install -y g++ cmake pkg-config \
  libboost-program-options-dev libtorrent-rasterbar-dev
mkdir build && cd build
cmake ..
make -j