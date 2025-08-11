# Plugins module - Built-in plugins
add_library(capturekit_plugins
    # TODO: Add plugin source files
    # plugin_manager.cpp
    # plugin_interface.cpp
)

target_include_directories(capturekit_plugins
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(capturekit_plugins
    PUBLIC
    capturekit_core
)

set_target_properties(capturekit_plugins PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
