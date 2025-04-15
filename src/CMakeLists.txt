# Define the main executable

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/gnc
)

# file(GLOB HEADERS "*.h" "gnc/*.h")
# file(GLOB SOURCES "*.cc" "gnc/*.cc")

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/gnc/control_thread.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/gnc/guidance_thread.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/gnc/navigation_thread.h
    ${CMAKE_CURRENT_SOURCE_DIR}/com_thread.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cyclic_thread_state_dependant.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/fcf_module_thread.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fsm_states.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/fsm_thread.h
    ${CMAKE_CURRENT_SOURCE_DIR}/god.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/log_thread.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared_memory.h
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gnc/control_thread.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/gnc/guidance_thread.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/gnc/navigation_thread.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/com_thread.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/fcf_module_thread.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/fsm_states.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/fsm_thread.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/god.cc
    # ${CMAKE_CURRENT_SOURCE_DIR}/log_thread.cc
    
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cc
)

add_executable(rocket ${SOURCES} ${HEADERS})

# Link to the Cactus RT library
# Add links for each integrations here
target_link_libraries(rocket PRIVATE control cactus_rt com_client json
    # guidance
)
