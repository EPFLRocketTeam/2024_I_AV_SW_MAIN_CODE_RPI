#!/bin/bash

# This script installs libprotobuff-dev version 3.21.12
# The compilation is run in the ~/protobuf_install directory    
# and the library is installed in the system path
# The ~/protobuf_install directory can be removed after installation

sudo apt-get install autoconf automake libtool curl make g++ unzip
mkdir -p ~/protobuf_install
cd ~/protobuf_install
curl -LO https://github.com/protocolbuffers/protobuf/releases/download/v21.12/protobuf-cpp-3.21.12.zip
unzip protobuf-cpp-3.21.12.zip
cd protobuf-3.21.12
./configure
make -j$(nproc) # $(nproc) ensures it uses all cores for compilation
sudo make install
sudo ldconfig # refresh shared library cache.