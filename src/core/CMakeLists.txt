# Core module - common utilities, settings, project I/O
add_library(capturekit_core
    settings.cpp
    project.cpp
    utils.cpp
    logger.cpp
)

target_include_directories(capturekit_core
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(capturekit_core
    PUBLIC
    Qt6::Core
    Qt6::Widgets
)

# Set target properties
set_target_properties(capturekit_core PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
