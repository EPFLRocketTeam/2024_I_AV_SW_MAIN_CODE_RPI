cmake_minimum_required(VERSION 3.10)
project(maincode)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add cactus_rt, a real-time library
add_subdirectory(cactus_rt)

### Add subdirectories for integrations here ##
add_subdirectory(control/drone_control)

# Add the main executable
add_subdirectory(src)
