# Render module - GPU preview, Skia/vector overlays
add_library(capturekit_render
    # TODO: Add render source files
    # gpu_preview.cpp
    # skia_overlays.cpp
    # shaders.cpp
)

target_include_directories(capturekit_render
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(capturekit_render
    PUBLIC
    capturekit_core
)

set_target_properties(capturekit_render PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
