# AI module - Whisper.cpp, OpenCV
add_library(capturekit_ai
    # TODO: Add AI source files
    # whisper_transcription.cpp
    # opencv_tracking.cpp
    # noise_suppression.cpp
)

target_include_directories(capturekit_ai
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(capturekit_ai
    PUBLIC
    capturekit_core
)

set_target_properties(capturekit_ai PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
