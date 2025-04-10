cmake_minimum_required(VERSION 3.10)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Option to enable cross-compilation for Raspberry Pi
option(CROSS_COMPILE_CM4 "Enable cross-compilation for Raspberry Pi Compute Module 4" OFF)

if(CROSS_COMPILE_CM4)
    # Toolchain configuration for Raspberry Pi
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR aarch64) # Correct for ARMv8 64-bit

    # Specify the 64-bit cross-compiler
    set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)

    # Specify the root path for Raspberry Pi libraries (adjust if necessary)
    set(CMAKE_FIND_ROOT_PATH /usr/aarch64-linux-gnu)

    # Search for libraries and includes in the target root path
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

    # Path to the copied CasADi install from the Pi
    set(casadi_DIR "${CMAKE_SOURCE_DIR}/casadi_arm64/casadi/install/lib/cmake/casadi")
    set(casadi_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/casadi_arm64/casadi/install/include")

    # Add CasADi ARM64 library path to linker search
    set(CASADI_LIB_PATH "${CMAKE_SOURCE_DIR}/casadi_arm64/casadi/install/lib")
    link_directories(${CASADI_LIB_PATH})

    message(STATUS "Cross-compilation for Raspberry Pi Compute Module 4 (ARMv8 64-bit) enabled")
else()
    message(STATUS "Compiling for the current machine")
endif()

option(ENABLE_TESTS "Enable test compilation" OFF)

project(maincode)

add_subdirectory(cactus_rt)
add_subdirectory(com_client)
add_subdirectory(json)
add_subdirectory(control/drone_control)
add_subdirectory(guidance)
add_subdirectory(src)

