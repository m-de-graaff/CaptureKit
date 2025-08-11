# CaptureKit

A professional-grade, cross-platform screen capture and video editing toolkit built with modern C++20, designed for content creators, educators, and developers who need high-performance video capture and editing capabilities.

## Features

- **High-Performance Capture**: Direct hardware API access for minimal latency
- **Cross-Platform**: Windows, macOS, and Linux support
- **Real-Time Editing**: GPU-accelerated preview with vector overlays
- **AI Integration**: Whisper.cpp for transcription, OpenCV for tracking
- **Professional Tools**: Timeline editing, scene graph, annotation tools
- **Plugin Architecture**: Extensible with C++ plugins and scripting

## Architecture

```
├─ src/
│  ├─ app/                # UI entry point (Qt6/QML)
│  ├─ capture/            # Screen/audio capture modules per OS
│  ├─ encoding/           # FFmpeg/libav integration
│  ├─ editor/             # Timeline, scene graph, annotation tools
│  ├─ render/             # GPU preview, Skia/vector overlays
│  ├─ ai/                 # Whisper.cpp, OpenCV integration
│  ├─ core/               # Common utilities, settings, project I/O
│  └─ plugins/            # Built-in plugins
├─ include/               # Public headers
├─ third_party/           # Dependencies (Skia, Whisper.cpp, nlohmann/json)
├─ cmake/                 # Toolchain files
├─ tests/                 # Unit & integration tests (Catch2)
└─ assets/                # Icons, branding
```

## Quick Start

### Prerequisites

- **C++20 compatible compiler** (GCC 10+, Clang 12+, MSVC 2019+)
- **CMake 3.20+**
- **Qt6** (Core, Widgets components)
- **Ninja** (recommended) or Make

### Windows Setup

1. **Install Visual Studio 2022** with C++ workload
2. **Install Qt6** via Qt installer or vcpkg
3. **Install build tools**:

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

# Or use the PowerShell script (Windows)
.\build.ps1 -BuildType release
```

### Available Build Presets

- `default` - Release build with Ninja
- `debug` - Debug build with symbols
- `release` - Release build with optimizations
- `msvc` - Visual Studio build

## Development

### Code Style

- **C++20** standard required
- **4 spaces** for indentation
- **120 character** line limit
- **snake_case** for variables and functions
- **PascalCase** for classes and namespaces

### Testing

```bash
# Build and run tests
cmake --build --preset default --target test

# Or run tests directly
ctest --preset default
```

### Code Quality Tools

- **clang-format** for code formatting
- **clang-tidy** for static analysis
- **cppcheck** for additional checks

```bash
# Format code
clang-format -i src/**/*.cpp include/**/*.hpp

# Run static analysis
clang-tidy src/**/*.cpp -checks=*
```

## Dependencies

### Core Dependencies

- **Qt6** - Cross-platform UI framework
- **FFmpeg** - Video/audio processing
- **Skia** - 2D graphics library
- **OpenCV** - Computer vision
- **Catch2** - Unit testing framework

### Platform-Specific

- **Windows**: DXGI Desktop Duplication API, WASAPI
- **macOS**: ScreenCaptureKit, CoreAudio
- **Linux**: PipeWire, X11/Wayland

## Configuration

### CMake Options

```bash
# Enable specific features
cmake -DENABLE_AI=ON -DENABLE_PLUGINS=ON ..

# Set Qt6 path
cmake -DCMAKE_PREFIX_PATH=/path/to/qt6 ..
```

### Environment Variables

```bash
# Set Qt6 installation path
export CMAKE_PREFIX_PATH=/path/to/qt6

# Set vcpkg toolchain
export CMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

## Documentation

- [Contributing Guide](CONTRIBUTING.md) - Development guidelines
- [Architecture Overview](docs/architecture.md) - System design
- [API Reference](docs/api.md) - Public API documentation
- [Plugin Development](docs/plugins.md) - Creating plugins

## Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details on:

- Setting up your development environment
- Code style and standards
- Testing requirements
- Pull request process

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- **Qt6** for the excellent cross-platform framework
- **FFmpeg** for robust media handling
- **Skia** for high-performance graphics
- **OpenCV** for computer vision capabilities

## Support

- **Issues**: [GitHub Issues](https://github.com/m-de-graaff/capturekit/issues)
- **Discussions**: [GitHub Discussions](https://github.com/m-de-graaff/capturekit/discussions)
- **Documentation**: [Project Wiki](https://github.com/m-de-graaff/capturekit/wiki)

---

**CaptureKit** - Professional screen capture and video editing toolkit 🎬✨
