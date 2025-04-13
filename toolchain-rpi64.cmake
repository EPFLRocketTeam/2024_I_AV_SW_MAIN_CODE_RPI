message(STATUS "Cross-compilation for Raspberry Pi Compute Module 4 (ARMv8 64-bit) enabled")

# Set target system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Set the cross compiler
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Set sysroot (important: point to the sysroot you copied)
set(SYSROOT_PATH ${CMAKE_SOURCE_DIR}/external_rpi_sysroot)
set(CMAKE_SYSROOT ${SYSROOT_PATH})

# Tell CMake where to look for headers and libs
set(CMAKE_FIND_ROOT_PATH ${SYSROOT_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Force pkg-config to use the sysroot version
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${SYSROOT_PATH})
set(ENV{PKG_CONFIG_PATH} "${SYSROOT_PATH}/usr/lib/aarch64-linux-gnu/pkgconfig:${SYSROOT_PATH}/usr/share/pkgconfig")

# Set the path to the sysroot libraries
set(CMAKE_LIBRARY_PATH "${SYSROOT_PATH}/usr/lib/aarch64-linux-gnu:${SYSROOT_PATH}/usr/lib:${SYSROOT_PATH}/usr/local/lib")
set(CMAKE_INCLUDE_PATH "${SYSROOT_PATH}/usr/include/aarch64-linux-gnu:${SYSROOT_PATH}/usr/include:${SYSROOT_PATH}/usr/local/include")
set(CMAKE_PREFIX_PATH "${SYSROOT_PATH}/usr/local")

# Protobuf config
set(Protobuf_INCLUDE_DIR "${SYSROOT_PATH}/usr/include")
set(Protobuf_LIBRARY "${SYSROOT_PATH}/usr/lib/aarch64-linux-gnu/libprotobuf.so")

# Casadi config
set(casadi_DIR "${SYSROOT_PATH}/usr/local/lib/cmake/casadi")
set(casadi_INCLUDE_DIRS "${SYSROOT_PATH}/usr/local/include")
