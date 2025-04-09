#!/bin/bash

# This script copies the compiled CasADi and IPOPT libraries from a Raspberry Pi to the local machine.
# It is intended to be run on a local machine after the libraries have been compiled on a Raspberry Pi.
# It assumes that the Raspberry Pi is accessible via SSH and that casadi_arm64 is the directory where the libraries are stored.
# This should be the case if install_casadi.sh was run on the Raspberry Pi.

INSTALLATION_DIR=$(pwd)
echo Installing into: $INSTALLATION_DIR

echo "🔧 Copying casadi from Raspberry Pi to local machine..."
mkdir -p casadi_arm64/CoinIpopt/install
scp -r pi@raspberrypi.local:casadi_arm64/casadi/install/ $INSTALLATION_DIR/casadi

echo "🔧 Copying CoinIpopt from Raspberry Pi to local machine..."
mkdir -p casadi_arm64/CoinIpopt/install
scp -r pi@raspberrypi.local:casadi_arm64/CoinIpopt/install/ $INSTALLATION_DIR/CoinIpopt

echo "Done!"