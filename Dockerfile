FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install basic build tools and cross-compilation toolchain
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    crossbuild-essential-arm64 \
    && rm -rf /var/lib/apt/lists/*

# Create workspace directory
WORKDIR /workspace

# Expose the external_rpi_sysroot directory location for CMake
ENV SYSROOT_PATH=/workspace/external_rpi_sysroot

# Set up environment variables for cross-compilation
ENV PKG_CONFIG_SYSROOT_DIR=${SYSROOT_PATH}
ENV PKG_CONFIG_PATH=${SYSROOT_PATH}/usr/lib/aarch64-linux-gnu/pkgconfig:${SYSROOT_PATH}/usr/share/pkgconfig

# Add helper script
COPY scripts/build.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/build.sh

CMD ["/bin/bash"]