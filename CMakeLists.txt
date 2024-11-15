cmake_minimum_required(VERSION 3.10)
project(maincode)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# We disbale tracing from cactus_rt to avoid needing two install protobufs
# This should be enaabled if tracing is needed
set(ENABLE_TRACING OFF CACHE BOOL "Enable runtime tracing support" FORCE)
add_subdirectory(cactus_rt)

### Add subdirectories for integrations here ##
add_subdirectory(control/drone_control)

# Add the main executable
add_subdirectory(src)
