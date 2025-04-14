#!/bin/bash

# This script installs all of the dependencies of the project on a raspberry pi.
# Installation takes around 2 to 3 hours, depending on the speed of your SD card and internet connection

# Needs to be run as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root using sudo."
    exit 1
fi

echo
echo "🔧 Installing required packages..."
echo

# Install required packages
# TODO: Lock versions of packages to avoid breaking changes
sudo apt-get update
sudo apt-get install -y build-essential gfortran liblapack-dev libmetis-dev libopenblas-dev protobuf-compiler libprotobuf-dev git cmake
sudo apt-get remove -y coinor-libipopt-dev


echo
echo "📦 Fetching and building IPOPT..."
echo

# Prepare the build directory
mkdir -p ~/casadi_arm64/CoinIpopt && cd ~/casadi_arm64/CoinIpopt

# Get the coinbrew script
wget https://raw.githubusercontent.com/coin-or/coinbrew/master/coinbrew
chmod u+x coinbrew

# Fetch IPOPT and dependencies
./coinbrew fetch Ipopt --no-prompt

# Build IPOPT
mkdir install
./coinbrew build Ipopt --prefix=install --test --no-prompt --verbosity=3


echo
echo "📥 Adding HSL solver..."
echo

# Download and extract HSL
cd ~/casadi_arm64/CoinIpopt/ThirdParty/HSL/
wget --no-check-certificate "https://drive.google.com/uc?export=download&id=1Y9IPknUFjZ62FrBfCXUWbxoauLoRkwXh" -O coinhsl-2023.11.17.zip
unzip coinhsl-2023.11.17.zip
mv coinhsl-2023.11.17 coinhsl

# Rebuild IPOPT with HSL support
cd ~/casadi_arm64/CoinIpopt
./coinbrew build Ipopt --prefix=install --test --no-prompt --verbosity=3
./coinbrew install Ipopt --no-prompt

# Create symbolic links for HSL
cd ~/casadi_arm64/CoinIpopt/install/lib
ln -s libcoinhsl.so libhsl.so


echo
echo "🔧 Updating .bashrc with CASADI configuration..."
echo

# Edit .bashrc to set environment variables
cat <<EOL >> ~/.bashrc
### CASADI config ###
export IPOPT_DIR=~/casadi_arm64/CoinIpopt/install
export PKG_CONFIG_PATH=\${PKG_CONFIG_PATH}:\${IPOPT_DIR}/lib/pkgconfig
export LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:\${IPOPT_DIR}/lib
export PATH=\${PATH}:\${IPOPT_DIR}/lib
export OMP_NUM_THREADS=1
### CASADI Config ###
EOL
source ~/.bashrc


echo
echo "⚙️ Cloning and building CasADi..."
echo

# Clone CasADi into the project directory
cd ~/casadi_arm64 && git clone https://github.com/casadi/casadi.git

# Create build directory
mkdir -p casadi/build && cd casadi/build

# Configure the build
cmake -DWITH_IPOPT:BOOL=ON \
      -DWITH_HSL:BOOL=ON \
      -DINCLUDE_PREFIX:PATH=include \
      -DCMAKE_PREFIX:PATH=lib/cmake/casadi \
      -DLIB_PREFIX:PATH=lib \
      -DBIN_PREFIX:PATH=bin ..

# Build CasADi
make -j4

# Install CasADi
make install

# Update the linker’s cache
sudo ldconfig


echo
echo "🚀 Dependencies installation complete!"
exit 0