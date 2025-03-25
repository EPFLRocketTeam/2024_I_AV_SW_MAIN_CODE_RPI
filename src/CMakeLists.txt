# Define the main executable

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/gnc
)

file(GLOB HEADERS "*.h" "gnc/*.h")
file(GLOB SOURCES "*.cc" "gnc/*.cc")

add_executable(rocket ${SOURCES} ${HEADERS})

# Link to the Cactus RT library
# Add links for each integrations here
target_link_libraries(rocket PRIVATE control cactus_rt com_client json)
