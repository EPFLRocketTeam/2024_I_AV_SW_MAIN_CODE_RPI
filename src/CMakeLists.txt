# Define the main executable

set(HEADERS
    control_interface.h
)

set(SOURCES
    control_interface.cc
    main.cc
)

add_executable(rocket ${SOURCES} ${HEADERS})

# Link to the Cactus RT library
# Add links for each integrations here
target_link_libraries(rocket PRIVATE control cactus_rt)
