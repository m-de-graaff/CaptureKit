# Main application executable
add_executable(CaptureKit
    main.cpp
    main_window.cpp
    application.cpp
)

target_include_directories(CaptureKit
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(CaptureKit
    PRIVATE
    capturekit_core
    capturekit_capture
    capturekit_encoding
    capturekit_editor
    capturekit_render
    capturekit_ai
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

# Set target properties
set_target_properties(CaptureKit PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    WIN32_EXECUTABLE TRUE
)

# Install target
install(TARGETS CaptureKit
    RUNTIME DESTINATION bin
)
