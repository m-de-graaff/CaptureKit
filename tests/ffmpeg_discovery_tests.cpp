#include <catch2/catch_test_macros.hpp>
#include "capturekit/encoding/ffmpeg_discovery.hpp"

using namespace capturekit;

TEST_CASE("FFmpegDiscovery basic functionality", "[encoding][ffmpeg]") {
    FFmpegDiscovery discovery;
    
    SECTION("Initial state") {
        REQUIRE_FALSE(discovery.is_available());
        REQUIRE(discovery.get_last_result().is_available == false);
    }
    
    SECTION("Discovery process") {
        auto result = discovery.discover();
        
        // Note: These tests may fail if FFmpeg is not available on the system
        // In a real CI environment, you'd want to mock FFmpeg or ensure it's available
        
        if (result.is_available) {
            REQUIRE(discovery.is_available());
            REQUIRE(result.error_message.empty());
            REQUIRE_FALSE(result.version.version_string.empty());
            REQUIRE_FALSE(result.codecs.empty());
            REQUIRE_FALSE(result.formats.empty());
        } else {
            REQUIRE_FALSE(discovery.is_available());
            REQUIRE_FALSE(result.error_message.empty());
        }
    }
}

TEST_CASE("FFmpegDiscovery hardware encoder detection", "[encoding][ffmpeg]") {
    FFmpegDiscovery discovery;
    
    SECTION("Hardware encoder availability check") {
        // Test each hardware encoder type
        auto encoders = {
            FFmpegDiscovery::HardwareEncoder::NVENC,
            FFmpegDiscovery::HardwareEncoder::QSV,
            FFmpegDiscovery::HardwareEncoder::AMF,
            FFmpegDiscovery::HardwareEncoder::VideoToolbox,
            FFmpegDiscovery::HardwareEncoder::VAAPI,
            FFmpegDiscovery::HardwareEncoder::VDPAU
        };
        
        for (auto encoder : encoders) {
            // This should not crash even if FFmpeg is not available
            REQUIRE_NOTHROW(discovery.is_hardware_encoder_available(encoder));
        }
    }
    
    SECTION("Available hardware encoders list") {
        auto available = discovery.get_available_hardware_encoders();
        REQUIRE(available.size() <= 6); // Maximum possible hardware encoders
        
        // If FFmpeg is available, we should get some results
        if (discovery.is_available()) {
            REQUIRE_FALSE(available.empty());
        }
    }
}

TEST_CASE("FFmpegDiscovery codec information", "[encoding][ffmpeg]") {
    FFmpegDiscovery discovery;
    
    SECTION("Video codec availability") {
        auto video_codecs = discovery.get_available_video_codecs();
        
        if (discovery.is_available()) {
            REQUIRE_FALSE(video_codecs.empty());
            
            // Check for common video codecs
            bool has_h264 = std::find(video_codecs.begin(), video_codecs.end(), "h264") != video_codecs.end();
            bool has_hevc = std::find(video_codecs.begin(), video_codecs.end(), "hevc") != video_codecs.end();
            
            // At least one of these should be available
            REQUIRE(has_h264 || has_hevc);
        }
    }
    
    SECTION("Audio codec availability") {
        auto audio_codecs = discovery.get_available_audio_codecs();
        
        if (discovery.is_available()) {
            REQUIRE_FALSE(audio_codecs.empty());
            
            // Check for common audio codecs
            bool has_aac = std::find(audio_codecs.begin(), audio_codecs.end(), "aac") != audio_codecs.end();
            bool has_mp3 = std::find(audio_codecs.begin(), audio_codecs.end(), "mp3") != audio_codecs.end();
            
            // At least one of these should be available
            REQUIRE(has_aac || has_mp3);
        }
    }
    
    SECTION("Codec info lookup") {
        if (discovery.is_available()) {
            auto h264_info = discovery.get_codec_info("h264");
            if (h264_info) {
                REQUIRE(h264_info->name == "h264");
                REQUIRE(h264_info->type == "video");
            }
            
            auto aac_info = discovery.get_codec_info("aac");
            if (aac_info) {
                REQUIRE(aac_info->name == "aac");
                REQUIRE(aac_info->type == "audio");
            }
        }
    }
}

TEST_CASE("FFmpegDiscovery format information", "[encoding][ffmpeg]") {
    FFmpegDiscovery discovery;
    
    SECTION("Output format availability") {
        auto output_formats = discovery.get_available_output_formats();
        
        if (discovery.is_available()) {
            REQUIRE_FALSE(output_formats.empty());
            
            // Check for common output formats
            bool has_mp4 = std::find(output_formats.begin(), output_formats.end(), "mp4") != output_formats.end();
            bool has_mkv = std::find(output_formats.begin(), output_formats.end(), "matroska") != output_formats.end();
            bool has_avi = std::find(output_formats.begin(), output_formats.end(), "avi") != output_formats.end();
            
            // At least one of these should be available
            REQUIRE(has_mp4 || has_mkv || has_avi);
        }
    }
    
    SECTION("Format info lookup") {
        if (discovery.is_available()) {
            auto mp4_info = discovery.get_format_info("mp4");
            if (mp4_info) {
                REQUIRE(mp4_info->name == "mp4");
                REQUIRE(mp4_info->can_mux);
            }
        }
    }
}

TEST_CASE("FFmpegDiscovery hardware capability information", "[encoding][ffmpeg]") {
    FFmpegDiscovery discovery;
    
    SECTION("Hardware capability lookup") {
        auto nvenc_cap = discovery.get_hardware_capability(FFmpegDiscovery::HardwareEncoder::NVENC);
        if (nvenc_cap) {
            REQUIRE(nvenc_cap->type == FFmpegDiscovery::HardwareEncoder::NVENC);
            REQUIRE(nvenc_cap->name == "NVIDIA NVENC");
            REQUIRE(nvenc_cap->description == "NVIDIA GPU-based hardware encoding");
        }
        
        auto qsv_cap = discovery.get_hardware_capability(FFmpegDiscovery::HardwareEncoder::QSV);
        if (qsv_cap) {
            REQUIRE(qsv_cap->type == FFmpegDiscovery::HardwareEncoder::QSV);
            REQUIRE(qsv_cap->name == "Intel Quick Sync Video");
            REQUIRE(qsv_cap->description == "Intel GPU-based hardware encoding");
        }
    }
}

TEST_CASE("FFmpegDiscovery error handling", "[encoding][ffmpeg]") {
    FFmpegDiscovery discovery;
    
    SECTION("Graceful handling when FFmpeg not available") {
        // These calls should not crash even when FFmpeg is not available
        REQUIRE_NOTHROW(discovery.get_available_video_codecs());
        REQUIRE_NOTHROW(discovery.get_available_audio_codecs());
        REQUIRE_NOTHROW(discovery.get_available_output_formats());
        REQUIRE_NOTHROW(discovery.get_available_hardware_encoders());
        
        // Should return empty results when not available
        if (!discovery.is_available()) {
            REQUIRE(discovery.get_available_video_codecs().empty());
            REQUIRE(discovery.get_available_audio_codecs().empty());
            REQUIRE(discovery.get_available_output_formats().empty());
            REQUIRE(discovery.get_available_hardware_encoders().empty());
        }
    }
}

TEST_CASE("FFmpegDiscovery version information", "[encoding][ffmpeg]") {
    FFmpegDiscovery discovery;
    
    SECTION("Version info structure") {
        auto result = discovery.discover();
        
        if (result.is_available) {
            REQUIRE_FALSE(result.version.version_string.empty());
            REQUIRE(result.version.major_version >= 0);
            REQUIRE(result.version.minor_version >= 0);
            REQUIRE(result.version.micro_version >= 0);
            
            // Version should be reasonable (not 0.0.0 unless it's a development build)
            if (result.version.major_version == 0 && result.version.minor_version == 0) {
                // This might be a development build, which is acceptable
                REQUIRE(true);
            } else {
                REQUIRE(result.version.major_version > 0);
            }
        }
    }
}

TEST_CASE("FFmpegDiscovery build configuration", "[encoding][ffmpeg]") {
    FFmpegDiscovery discovery;
    
    SECTION("Build config information") {
        auto result = discovery.discover();
        
        if (result.is_available) {
            REQUIRE_FALSE(result.build_config.empty());
            
            // Should have platform information
            auto platform_it = result.build_config.find("platform");
            if (platform_it != result.build_config.end()) {
                std::string platform = platform_it->second;
                REQUIRE((platform == "Windows" || platform == "macOS" || platform == "Linux"));
            }
        }
    }
}
