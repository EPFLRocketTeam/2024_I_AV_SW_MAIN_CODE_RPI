file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cc")

add_library(com_client ${SOURCES} ${HEADERS})
target_link_libraries(com_client PUBLIC quill::quill)

target_include_directories(com_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})