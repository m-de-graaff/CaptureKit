# Editor module - Timeline, scene graph, annotation tools
add_library(capturekit_editor
    # TODO: Add editor source files
    # timeline.cpp
    # scene_graph.cpp
    # annotation_tools.cpp
)

target_include_directories(capturekit_editor
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(capturekit_editor
    PUBLIC
    capturekit_core
)

set_target_properties(capturekit_editor PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
