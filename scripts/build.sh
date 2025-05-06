#!/bin/bash
# Helper script for cross-compilation inside the Docker container

if [ ! -d "external_rpi_sysroot" ]; then
    echo "Error: external_rpi_sysroot directory not found!"
    echo "Make sure you've mounted your project correctly with the sysroot directory."
    exit 1
fi

# Create build directory if it doesn't exist
mkdir -p build_cm4
cd build_cm4

# Configure with CMake using the toolchain file
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-rpi64.cmake ..

# Build the project
make -j$(nproc)

echo "Build complete!"