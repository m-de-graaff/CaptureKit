#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>

namespace capturekit {

/**
 * @brief FFmpeg/libav runtime discovery and capability probing
 * 
 * This class handles:
 * - Runtime discovery of FFmpeg/libav libraries
 * - Version checking and compatibility verification
 * - Hardware encoder capability probing (NVENC, QSV, AMF, VideoToolbox)
 * - Codec and format availability detection
 */
class FFmpegDiscovery {
public:
    /**
     * @brief Hardware encoder types supported by FFmpeg
     */
    enum class HardwareEncoder {
        None,
        NVENC,      // NVIDIA GPU encoding
        QSV,        // Intel Quick Sync Video
        AMF,        // AMD Advanced Media Framework
        VideoToolbox, // macOS VideoToolbox
        VAAPI,      // Linux Video Acceleration API
        VDPAU       // Linux Video Decode and Presentation API
    };

    /**
     * @brief Codec information structure
     */
    struct CodecInfo {
        std::string name;
        std::string long_name;
        std::string type;  // "video", "audio", "subtitle"
        bool is_encoder;
        bool is_decoder;
        bool is_hardware_accelerated;
        HardwareEncoder hardware_type{HardwareEncoder::None};
        std::vector<std::string> supported_formats;
    };

    /**
     * @brief Format information structure
     */
    struct FormatInfo {
        std::string name;
        std::string long_name;
        std::string extensions;
        std::string mime_type;
        bool can_demux;
        bool can_mux;
    };

    /**
     * @brief Hardware encoder capability information
     */
    struct HardwareCapability {
        HardwareEncoder type;
        std::string name;
        std::string description;
        std::vector<std::string> supported_codecs;
        std::vector<std::string> supported_pixel_formats;
        std::vector<int> supported_resolutions;
        std::vector<int> supported_framerates;
        bool is_available;
        std::string error_message;
    };

    /**
     * @brief FFmpeg version information
     */
    struct VersionInfo {
        std::string version_string;
        int major_version{0};
        int minor_version{0};
        int micro_version{0};
        std::string build_date;
        std::string build_time;
        std::string configuration;
        std::string libavutil_version;
        std::string libavcodec_version;
        std::string libavformat_version;
        std::string libswscale_version;
    };

    /**
     * @brief Discovery result containing all information
     */
    struct DiscoveryResult {
        bool is_available{false};
        std::string error_message;
        VersionInfo version;
        std::vector<CodecInfo> codecs;
        std::vector<FormatInfo> formats;
        std::vector<HardwareCapability> hardware_encoders;
        std::unordered_map<std::string, std::string> build_config;
    };

    FFmpegDiscovery() = default;
    ~FFmpegDiscovery() = default;

    // Non-copyable, non-movable
    FFmpegDiscovery(const FFmpegDiscovery&) = delete;
    FFmpegDiscovery& operator=(const FFmpegDiscovery&) = delete;
    FFmpegDiscovery(FFmpegDiscovery&&) = delete;
    FFmpegDiscovery& operator=(FFmpegDiscovery&&) = delete;

    /**
     * @brief Discover FFmpeg/libav runtime capabilities
     * @return Discovery result with all available information
     */
    DiscoveryResult discover();

    /**
     * @brief Check if FFmpeg is available at runtime
     * @return true if FFmpeg is available and functional
     */
    bool is_available() const;

    /**
     * @brief Get the last discovery result
     * @return Last discovery result or empty result if not discovered
     */
    const DiscoveryResult& get_last_result() const;

    /**
     * @brief Check if a specific hardware encoder is available
     * @param encoder_type Hardware encoder type to check
     * @return true if the hardware encoder is available
     */
    bool is_hardware_encoder_available(HardwareEncoder encoder_type) const;

    /**
     * @brief Get information about a specific codec
     * @param codec_name Name of the codec to look up
     * @return Codec information if found, std::nullopt otherwise
     */
    std::optional<CodecInfo> get_codec_info(const std::string& codec_name) const;

    /**
     * @brief Get information about a specific format
     * @param format_name Name of the format to look up
     * @return Format information if found, std::nullopt otherwise
     */
    std::optional<FormatInfo> get_format_info(const std::string& format_name) const;

    /**
     * @brief Get hardware encoder capabilities for a specific type
     * @param encoder_type Hardware encoder type
     * @return Hardware capability information if found, std::nullopt otherwise
     */
    std::optional<HardwareCapability> get_hardware_capability(HardwareEncoder encoder_type) const;

    /**
     * @brief Get list of available hardware encoders
     * @return Vector of available hardware encoder types
     */
    std::vector<HardwareEncoder> get_available_hardware_encoders() const;

    /**
     * @brief Get list of available video codecs
     * @return Vector of available video codec names
     */
    std::vector<std::string> get_available_video_codecs() const;

    /**
     * @brief Get list of available audio codecs
     * @return Vector of available audio codec names
     */
    std::vector<std::string> get_available_audio_codecs() const;

    /**
     * @brief Get list of available output formats
     * @return Vector of available output format names
     */
    std::vector<std::string> get_available_output_formats() const;

private:
    /**
     * @brief Initialize FFmpeg libraries
     * @return true if initialization successful
     */
    bool initialize_ffmpeg();

    /**
     * @brief Discover version information
     * @return Version information structure
     */
    VersionInfo discover_version_info();

    /**
     * @brief Discover available codecs
     * @return Vector of codec information
     */
    std::vector<CodecInfo> discover_codecs();

    /**
     * @brief Discover available formats
     * @return Vector of format information
     */
    std::vector<FormatInfo> discover_formats();

    /**
     * @brief Discover hardware encoder capabilities
     * @return Vector of hardware capability information
     */
    std::vector<HardwareCapability> discover_hardware_encoders();

    /**
     * @brief Probe specific hardware encoder
     * @param encoder_type Type of hardware encoder to probe
     * @return Hardware capability information
     */
    HardwareCapability probe_hardware_encoder(HardwareEncoder encoder_type);

    /**
     * @brief Probe NVENC capabilities
     * @return Hardware capability information for NVENC
     */
    HardwareCapability probe_nvenc();

    /**
     * @brief Probe Intel QSV capabilities
     * @return Hardware capability information for QSV
     */
    HardwareCapability probe_qsv();

    /**
     * @brief Probe AMD AMF capabilities
     * @return Hardware capability information for AMF
     */
    HardwareCapability probe_amf();

    /**
     * @brief Probe macOS VideoToolbox capabilities
     * @return Hardware capability information for VideoToolbox
     */
    HardwareCapability probe_videotoolbox();

    /**
     * @brief Probe Linux VAAPI capabilities
     * @return Hardware capability information for VAAPI
     */
    HardwareCapability probe_vaapi();

    /**
     * @brief Probe Linux VDPAU capabilities
     * @return Hardware capability information for VDPAU
     */
    HardwareCapability probe_vdpau();

    /**
     * @brief Get build configuration information
     * @return Map of build configuration keys and values
     */
    std::unordered_map<std::string, std::string> get_build_config();

    DiscoveryResult last_result_;
    bool is_initialized_{false};
};

} // namespace capturekit
