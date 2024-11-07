cmake_minimum_required(VERSION 3.10)
project(rocket)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
file(GLOB SOURCES "${SRC_DIR}/*.cc")
file(GLOB HEADERS "${SRC_DIR}/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} pthread)
