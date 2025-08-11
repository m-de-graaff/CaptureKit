# Encoding module - FFmpeg/libav integration
add_library(capturekit_encoding
    ffmpeg_discovery.cpp
    # TODO: Add more encoding source files
    # encoder.cpp
    # muxer.cpp
)

target_include_directories(capturekit_encoding
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(capturekit_encoding
    PUBLIC
    capturekit_core
)

set_target_properties(capturekit_encoding PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
