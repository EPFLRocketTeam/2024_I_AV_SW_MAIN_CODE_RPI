# Define the main executable

file(GLOB HEADERS "*.h")

file(GLOB SOURCES "*.cc")

add_executable(rocket ${SOURCES} ${HEADERS})

# Link to the Cactus RT library
# Add links for each integrations here
target_link_libraries(rocket PRIVATE control cactus_rt)
