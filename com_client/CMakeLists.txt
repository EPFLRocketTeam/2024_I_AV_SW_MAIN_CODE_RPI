file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cc")

add_library(com_client ${SOURCES} ${HEADERS})

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(com_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})