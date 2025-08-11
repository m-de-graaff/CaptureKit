# Tests configuration
find_package(Catch2 3 REQUIRED)

# Enable CTest integration
include(CTest)
enable_testing()

# Add test executable
add_executable(CaptureKitTests
    main.cpp
    core_tests.cpp
    capture_tests.cpp
    encoding_tests.cpp
)

# Link with Catch2 and project libraries
target_link_libraries(CaptureKitTests
    PRIVATE
    Catch2::Catch2
    capturekit_core
    capturekit_capture
    capturekit_encoding
)

# Add tests to CTest
include(AddCatchTests)
add_catch_tests(CaptureKitTests)
