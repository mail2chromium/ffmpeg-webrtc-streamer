#!/bin/bash

# Copyright 2022 ottoman, All Rights Reserved.

sudo su

echo "Streamer setup has been initiated..."

sudo apt update -y
sudo apt install build-essential -y
sudo apt install g++ -y
sudo apt install gcc -y
sudo apt install git -y
sudo apt-get update && sudo apt-get -y install git python -y

echo "\nBuild Essentials have been installed.\n"
echo "Working on other necessary packages.\n"

sudo apt-get install libfdk-aac-dev -y
sudo apt-get install libboost-all-dev -y
sudo apt-get install libssl-dev -y
sudo apt-get -y install libcrypto++6 
sudo apt-get install libpthread-dev -y
sudo apt-get install libpthread-stubs0-dev -y
sudo apt-get install libxcb-xfixes0-dev -y
sudo apt-get install libsdl-dev libxcb1-dev libxcb-image0-dev libxcb-icccm1-dev libxcb-keysyms1-dev -y
sudo apt-get install libstdc++6-4.6-dev -y
sudo apt-get install libstdc++-dev -y
sudo apt-get install libstdc++-4.8-dev -y
sudo apt-get install libwebsocketpp-dev -y
sudo apt-get install nlohmann-json-dev -y
sudo apt-get install nlohmann-json3-dev -y
sudo apt-get install libsndio-dev -y
sudo apt-get install -y clang libc++-dev libc++abi-dev -y
sudo apt install libx11-dev -y
sudo apt install libxinerama-dev -y
sudo apt install libxft-dev -y
sudo apt install libx11-xcb-dev -y
sudo apt install libxcb-res0-dev -y
sudo apt-get install libxcb-xrm-dev -y
sudo apt-get install libxcb-shm-dev -y
sudo apt-get install libsdl-dev libxcb1-dev libxcb-image0-dev libxcb-icccm4-dev libxcb-keysyms1-dev -y
sudo apt-get install libxcb-xfixes0-dev -y
sudo apt-get install liblzma-dev -y

sudo apt-get install nasm
rm -rf ./x264
git -C x264 pull 2> /dev/null || git clone --depth 1 https://code.videolan.org/videolan/x264.git && cd x264 && PATH="./bin:$PATH" PKG_CONFIG_PATH="./ffmpeg_build/lib/pkgconfig" ./configure --prefix="./ffmpeg_build" --bindir="./bin" --enable-static --enable-pic && PATH="./bin:$PATH" make && make install
