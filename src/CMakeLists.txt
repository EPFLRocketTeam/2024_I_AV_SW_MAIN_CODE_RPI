# Define the main executable
add_executable(rocket main.cc)

# Link to the Cactus RT library
# Add links for each integrations here
target_link_libraries(rocket PRIVATE cactus_rt)
