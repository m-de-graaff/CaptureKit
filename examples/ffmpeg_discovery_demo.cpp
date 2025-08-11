#include <iostream>
#include <iomanip>
#include <string>
#include "capturekit/encoding/ffmpeg_discovery.hpp"

using namespace capturekit;

void print_separator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void print_version_info(const FFmpegDiscovery::VersionInfo& version) {
    std::cout << "FFmpeg Version: " << version.version_string << "\n";
    std::cout << "Major: " << version.major_version 
              << ", Minor: " << version.minor_version 
              << ", Micro: " << version.micro_version << "\n";
    std::cout << "Build Date: " << version.build_date << "\n";
    std::cout << "Configuration: " << version.configuration << "\n";
}

void print_codec_info(const FFmpegDiscovery::CodecInfo& codec) {
    std::cout << "  " << std::setw(20) << std::left << codec.name
              << " | " << std::setw(8) << std::left << codec.type
              << " | " << (codec.is_encoder ? "E" : "-")
              << (codec.is_decoder ? "D" : "-")
              << (codec.is_hardware_accelerated ? "H" : "-")
              << " | " << codec.long_name << "\n";
}

void print_format_info(const FFmpegDiscovery::FormatInfo& format) {
    std::cout << "  " << std::setw(15) << std::left << format.name
              << " | " << std::setw(10) << std::left << format.extensions
              << " | " << (format.can_demux ? "D" : "-")
              << (format.can_mux ? "M" : "-")
              << " | " << format.long_name << "\n";
}

void print_hardware_capability(const FFmpegDiscovery::HardwareCapability& cap) {
    std::cout << "  " << std::setw(20) << std::left << cap.name
              << " | " << (cap.is_available ? "Available" : "Not Available")
              << " | " << cap.description << "\n";
    
    if (cap.is_available) {
        std::cout << "    Supported Codecs: ";
        for (const auto& codec : cap.supported_codecs) {
            std::cout << codec << " ";
        }
        std::cout << "\n";
        
        std::cout << "    Supported Formats: ";
        for (const auto& format : cap.supported_pixel_formats) {
            std::cout << format << " ";
        }
        std::cout << "\n";
        
        std::cout << "    Max Resolution: ";
        if (!cap.supported_resolutions.empty()) {
            std::cout << cap.supported_resolutions.back() << "p";
        }
        std::cout << "\n";
        
        std::cout << "    Max Framerate: ";
        if (!cap.supported_framerates.empty()) {
            std::cout << cap.supported_framerates.back() << "fps";
        }
        std::cout << "\n";
    } else if (!cap.error_message.empty()) {
        std::cout << "    Error: " << cap.error_message << "\n";
    }
}

int main() {
    std::cout << "CaptureKit FFmpeg Discovery Demo\n";
    std::cout << "================================\n\n";
    
    FFmpegDiscovery discovery;
    
    print_separator("Discovering FFmpeg Capabilities");
    
    auto result = discovery.discover();
    
    if (!result.is_available) {
        std::cout << "❌ FFmpeg is not available on this system.\n";
        std::cout << "Error: " << result.error_message << "\n";
        std::cout << "\nPlease ensure FFmpeg is properly installed and linked.\n";
        return 1;
    }
    
    std::cout << "✅ FFmpeg is available and functional!\n\n";
    
    // Print version information
    print_separator("Version Information");
    print_version_info(result.version);
    
    // Print available codecs
    print_separator("Available Codecs");
    std::cout << "  " << std::setw(20) << std::left << "Name"
              << " | " << std::setw(8) << std::left << "Type"
              << " | E/D/H | Description\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& codec : result.codecs) {
        if (codec.is_encoder || codec.is_decoder) {
            print_codec_info(codec);
        }
    }
    
    // Print available formats
    print_separator("Available Formats");
    std::cout << "  " << std::setw(15) << std::left << "Name"
              << " | " << std::setw(10) << std::left << "Extensions"
              << " | D/M | Description\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& format : result.formats) {
        if (format.can_mux || format.can_demux) {
            print_format_info(format);
        }
    }
    
    // Print hardware encoder capabilities
    print_separator("Hardware Encoder Capabilities");
    std::cout << "  " << std::setw(20) << std::left << "Name"
              << " | Status      | Description\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& cap : result.hardware_encoders) {
        print_hardware_capability(cap);
        std::cout << "\n";
    }
    
    // Print build configuration
    print_separator("Build Configuration");
    for (const auto& [key, value] : result.build_config) {
        std::cout << "  " << std::setw(20) << std::left << key << ": " << value << "\n";
    }
    
    // Summary
    print_separator("Summary");
    std::cout << "Total Codecs: " << result.codecs.size() << "\n";
    std::cout << "Total Formats: " << result.formats.size() << "\n";
    
    auto available_hw = discovery.get_available_hardware_encoders();
    std::cout << "Available Hardware Encoders: " << available_hw.size() << "\n";
    
    if (!available_hw.empty()) {
        std::cout << "Hardware Encoders: ";
        for (auto encoder : available_hw) {
            switch (encoder) {
                case FFmpegDiscovery::HardwareEncoder::NVENC:
                    std::cout << "NVENC ";
                    break;
                case FFmpegDiscovery::HardwareEncoder::QSV:
                    std::cout << "QSV ";
                    break;
                case FFmpegDiscovery::HardwareEncoder::AMF:
                    std::cout << "AMF ";
                    break;
                case FFmpegDiscovery::HardwareEncoder::VideoToolbox:
                    std::cout << "VideoToolbox ";
                    break;
                case FFmpegDiscovery::HardwareEncoder::VAAPI:
                    std::cout << "VAAPI ";
                    break;
                case FFmpegDiscovery::HardwareEncoder::VDPAU:
                    std::cout << "VDPAU ";
                    break;
                default:
                    std::cout << "Unknown ";
                    break;
            }
        }
        std::cout << "\n";
    }
    
    auto video_codecs = discovery.get_available_video_codecs();
    auto audio_codecs = discovery.get_available_audio_codecs();
    auto output_formats = discovery.get_available_output_formats();
    
    std::cout << "Video Encoders: " << video_codecs.size() << "\n";
    std::cout << "Audio Encoders: " << audio_codecs.size() << "\n";
    std::cout << "Output Formats: " << output_formats.size() << "\n";
    
    std::cout << "\n🎉 FFmpeg discovery completed successfully!\n";
    
    return 0;
}
