# Capture module - Screen/audio capture modules per OS
add_library(capturekit_capture
    # TODO: Add capture source files
    # screen_capture.cpp
    # audio_capture.cpp
)

target_include_directories(capturekit_capture
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(capturekit_capture
    PUBLIC
    capturekit_core
)

set_target_properties(capturekit_capture PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
