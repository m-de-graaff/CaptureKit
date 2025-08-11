# Contributing to CaptureKit

Thank you for your interest in contributing to CaptureKit! This document provides guidelines and information for contributors.

## Development Environment Setup

### Prerequisites

- **C++20 compatible compiler** (GCC 10+, Clang 12+, MSVC 2019+)
- **CMake 3.20+**
- **Qt6** (Core, Widgets components)
- **Ninja** (recommended) or Make
- **Git**

### Windows Setup

1. Install Visual Studio 2022 with C++ workload
2. Install Qt6 via Qt installer or vcpkg
3. Install CMake and Ninja via chocolatey or vcpkg

```powershell
# Using chocolatey
choco install cmake ninja

# Or using vcpkg
vcpkg install cmake ninja
```

### Building from Source

```bash
# Configure with default preset
cmake --preset default

# Build
cmake --build --preset default

# Or use the build preset directly
cmake --build --preset default
```

## Code Style and Standards

### C++ Coding Standards

- **C++20** standard required
- **4 spaces** for indentation (no tabs)
- **120 character** line limit
- **snake_case** for variables and functions
- **PascalCase** for classes and namespaces
- **UPPER_CASE** for constants and macros

### File Organization

- Header files in `include/` directory
- Implementation files in `src/` directory
- One class per header file
- Include guards using `#pragma once`

### Example Code Style

```cpp
#pragma once

#include <memory>
#include <string>

namespace capturekit {

class ScreenCapture {
public:
    explicit ScreenCapture(const std::string& display_name);
    ~ScreenCapture() = default;

    bool initialize();
    std::unique_ptr<Frame> capture_frame();

private:
    std::string display_name_;
    bool initialized_{false};
};

} // namespace capturekit
```

## Testing

### Test Framework

We use **Catch2** for unit testing. All new features must include tests.

### Running Tests

```bash
# Build and run tests
cmake --build --preset default --target test

# Or run tests directly
ctest --preset default
```

### Writing Tests

```cpp
#include <catch2/catch_test_macros.hpp>
#include "capturekit/screen_capture.hpp"

TEST_CASE("ScreenCapture initialization", "[capture]") {
    capturekit::ScreenCapture capture(":0");
    REQUIRE(capture.initialize() == true);
}
```

## Code Quality Tools

### Static Analysis

- **clang-tidy** for static analysis
- **cppcheck** for additional checks
- **clang-format** for code formatting

### Pre-commit Setup

```bash
# Install pre-commit hooks
pre-commit install

# Run all checks
pre-commit run --all-files
```

### Manual Code Formatting

```bash
# Format code with clang-format
clang-format -i src/**/*.cpp include/**/*.hpp

# Run clang-tidy
clang-tidy src/**/*.cpp -checks=*
```

## Pull Request Process

1. **Fork** the repository
2. **Create** a feature branch from `main`
3. **Make** your changes following the coding standards
4. **Add** tests for new functionality
5. **Ensure** all tests pass
6. **Update** documentation if needed
7. **Submit** a pull request

### Pull Request Checklist

- [ ] Code follows style guidelines
- [ ] Tests added and passing
- [ ] Documentation updated
- [ ] No compiler warnings
- [ ] Static analysis passes
- [ ] Cross-platform compatibility verified

## Commit Message Format

Use conventional commit format:

```
type(scope): description

[optional body]

[optional footer]
```

Examples:
- `feat(capture): add DXGI desktop duplication support`
- `fix(editor): resolve timeline crash on large projects`
- `docs(readme): update build instructions for Windows`

## Architecture Guidelines

### Module Design

- **Low coupling** between modules
- **High cohesion** within modules
- **Interface segregation** principle
- **Dependency injection** for testability

### Error Handling

- Use exceptions for exceptional cases
- Return `std::expected` or `std::optional` for expected failures
- Log errors with appropriate levels
- Provide meaningful error messages

### Memory Management

- Prefer RAII and smart pointers
- Avoid raw pointers for ownership
- Use `std::unique_ptr` for exclusive ownership
- Use `std::shared_ptr` only when shared ownership is needed

## Performance Guidelines

- Profile before optimizing
- Use move semantics where appropriate
- Minimize allocations in hot paths
- Consider cache locality in data structures
- Use appropriate data structures for the use case

## Platform Support

### Windows

- Test on Windows 10/11
- Support both MSVC and MinGW
- Handle Windows-specific APIs gracefully

### macOS

- Test on macOS 12.3+ (ScreenCaptureKit requirement)
- Support both Intel and Apple Silicon
- Handle sandboxing restrictions

### Linux

- Test on Ubuntu 20.04+ and recent distributions
- Support both X11 and Wayland
- Handle different audio backends (PulseAudio, PipeWire)

## Getting Help

- **Issues**: Use GitHub issues for bugs and feature requests
- **Discussions**: Use GitHub discussions for questions and ideas
- **Code Review**: Request reviews from maintainers
- **Documentation**: Check existing docs and examples

## License

By contributing to CaptureKit, you agree that your contributions will be licensed under the same license as the project.

---

Thank you for contributing to CaptureKit! 🎥✨
