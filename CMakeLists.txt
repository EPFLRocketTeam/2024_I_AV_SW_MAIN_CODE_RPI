cmake_minimum_required(VERSION 3.10)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(Protobuf_PROTOC_EXECUTABLE "${CMAKE_SOURCE_DIR}/protoc/bin/protoc")

# Option for the compilation
option(ENABLE_TESTS "Enable test compilation" OFF)
option(CROSS_COMPILE_CM4 "Enable cross-compilation for Raspberry Pi Compute Module 4" OFF)

if(CROSS_COMPILE_CM4)
message(FATAL_ERROR "The option -DCROSS_COMPILE_CM4=ON is no longer used to cross-compile the project!
To cross-compile, run this command instead:
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-rpi64.cmake ..
See README.md for more information.")
endif()

project(maincode)

add_subdirectory(cactus_rt)
add_subdirectory(com_client)
add_subdirectory(json)
add_subdirectory(control/drone_control)
add_subdirectory(guidance)
add_subdirectory(src)

