#include "capturekit/encoding/ffmpeg_discovery.hpp"

// FFmpeg headers
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/hwcontext.h>
#include <libavutil/hwcontext_d3d11va.h>
#include <libavutil/hwcontext_dxva2.h>
#include <libavutil/hwcontext_qsv.h>
#include <libavutil/hwcontext_vaapi.h>
#include <libavutil/hwcontext_vdpau.h>
}

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <VideoToolbox/VideoToolbox.h>
#elif defined(__linux__)
#include <va/va.h>
#include <va/va_version.h>
#include <vdpau/vdpau.h>
#include <vdpau/vdpau_x11.h>
#endif

namespace capturekit {

FFmpegDiscovery::DiscoveryResult FFmpegDiscovery::discover() {
    if (!is_initialized_ && !initialize_ffmpeg()) {
        last_result_.is_available = false;
        last_result_.error_message = "Failed to initialize FFmpeg libraries";
        return last_result_;
    }

    last_result_.is_available = true;
    last_result_.error_message.clear();

    try {
        last_result_.version = discover_version_info();
        last_result_.codecs = discover_codecs();
        last_result_.formats = discover_formats();
        last_result_.hardware_encoders = discover_hardware_encoders();
        last_result_.build_config = get_build_config();
    } catch (const std::exception& e) {
        last_result_.is_available = false;
        last_result_.error_message = std::string("Discovery failed: ") + e.what();
    }

    return last_result_;
}

bool FFmpegDiscovery::is_available() const {
    return is_initialized_ && last_result_.is_available;
}

const FFmpegDiscovery::DiscoveryResult& FFmpegDiscovery::get_last_result() const {
    return last_result_;
}

bool FFmpegDiscovery::is_hardware_encoder_available(HardwareEncoder encoder_type) const {
    if (!is_available()) {
        return false;
    }

    auto it = std::find_if(last_result_.hardware_encoders.begin(),
                           last_result_.hardware_encoders.end(),
                           [encoder_type](const HardwareCapability& cap) {
                               return cap.type == encoder_type && cap.is_available;
                           });
    return it != last_result_.hardware_encoders.end();
}

std::optional<FFmpegDiscovery::CodecInfo> FFmpegDiscovery::get_codec_info(const std::string& codec_name) const {
    if (!is_available()) {
        return std::nullopt;
    }

    auto it = std::find_if(last_result_.codecs.begin(),
                           last_result_.codecs.end(),
                           [&codec_name](const CodecInfo& info) {
                               return info.name == codec_name;
                           });
    
    if (it != last_result_.codecs.end()) {
        return *it;
    }
    return std::nullopt;
}

std::optional<FFmpegDiscovery::FormatInfo> FFmpegDiscovery::get_format_info(const std::string& format_name) const {
    if (!is_available()) {
        return std::nullopt;
    }

    auto it = std::find_if(last_result_.formats.begin(),
                           last_result_.formats.end(),
                           [&format_name](const FormatInfo& info) {
                               return info.name == format_name;
                           });
    
    if (it != last_result_.formats.end()) {
        return *it;
    }
    return std::nullopt;
}

std::optional<FFmpegDiscovery::HardwareCapability> FFmpegDiscovery::get_hardware_capability(HardwareEncoder encoder_type) const {
    if (!is_available()) {
        return std::nullopt;
    }

    auto it = std::find_if(last_result_.hardware_encoders.begin(),
                           last_result_.hardware_encoders.end(),
                           [encoder_type](const HardwareCapability& cap) {
                               return cap.type == encoder_type;
                           });
    
    if (it != last_result_.hardware_encoders.end()) {
        return *it;
    }
    return std::nullopt;
}

std::vector<FFmpegDiscovery::HardwareEncoder> FFmpegDiscovery::get_available_hardware_encoders() const {
    std::vector<HardwareEncoder> available;
    
    if (!is_available()) {
        return available;
    }

    for (const auto& cap : last_result_.hardware_encoders) {
        if (cap.is_available) {
            available.push_back(cap.type);
        }
    }
    
    return available;
}

std::vector<std::string> FFmpegDiscovery::get_available_video_codecs() const {
    std::vector<std::string> video_codecs;
    
    if (!is_available()) {
        return video_codecs;
    }

    for (const auto& codec : last_result_.codecs) {
        if (codec.type == "video" && codec.is_encoder) {
            video_codecs.push_back(codec.name);
        }
    }
    
    return video_codecs;
}

std::vector<std::string> FFmpegDiscovery::get_available_audio_codecs() const {
    std::vector<std::string> audio_codecs;
    
    if (!is_available()) {
        return audio_codecs;
    }

    for (const auto& codec : last_result_.codecs) {
        if (codec.type == "audio" && codec.is_encoder) {
            audio_codecs.push_back(codec.name);
        }
    }
    
    return audio_codecs;
}

std::vector<std::string> FFmpegDiscovery::get_available_output_formats() const {
    std::vector<std::string> output_formats;
    
    if (!is_available()) {
        return output_formats;
    }

    for (const auto& format : last_result_.formats) {
        if (format.can_mux) {
            output_formats.push_back(format.name);
        }
    }
    
    return output_formats;
}

bool FFmpegDiscovery::initialize_ffmpeg() {
    try {
        // Initialize FFmpeg libraries
        av_log_set_level(AV_LOG_QUIET); // Suppress FFmpeg logging during discovery
        
        // Test basic functionality
        if (avcodec_find_decoder(AV_CODEC_ID_H264) == nullptr) {
            return false;
        }
        
        is_initialized_ = true;
        return true;
    } catch (...) {
        return false;
    }
}

FFmpegDiscovery::VersionInfo FFmpegDiscovery::discover_version_info() {
    VersionInfo info;
    
    // Get FFmpeg version
    info.version_string = av_version_info();
    
    // Parse version string (format: "major.minor.micro")
    std::istringstream version_stream(info.version_string);
    std::string version_part;
    
    if (std::getline(version_stream, version_part, '.')) {
        info.major_version = std::stoi(version_part);
    }
    if (std::getline(version_stream, version_part, '.')) {
        info.minor_version = std::stoi(version_part);
    }
    if (std::getline(version_stream, version_part, '.')) {
        info.micro_version = std::stoi(version_part);
    }
    
    // Get library versions
    info.libavutil_version = av_version_info();
    info.libavcodec_version = av_version_info();
    info.libavformat_version = av_version_info();
    info.libswscale_version = av_version_info();
    
    // Get build configuration
    info.configuration = avutil_configuration();
    info.build_date = avutil_license();
    
    return info;
}

std::vector<FFmpegDiscovery::CodecInfo> FFmpegDiscovery::discover_codecs() {
    std::vector<CodecInfo> codecs;
    
    void* opaque = nullptr;
    const AVCodec* codec = nullptr;
    
    while ((codec = av_codec_iterate(&opaque)) != nullptr) {
        CodecInfo info;
        info.name = codec->name ? codec->name : "";
        info.long_name = codec->long_name ? codec->long_name : "";
        info.is_encoder = av_codec_is_encoder(codec) != 0;
        info.is_decoder = av_codec_is_decoder(codec) != 0;
        
        // Determine codec type
        if (codec->type == AVMEDIA_TYPE_VIDEO) {
            info.type = "video";
        } else if (codec->type == AVMEDIA_TYPE_AUDIO) {
            info.type = "audio";
        } else if (codec->type == AVMEDIA_TYPE_SUBTITLE) {
            info.type = "subtitle";
        } else {
            info.type = "unknown";
        }
        
        // Check if hardware accelerated
        info.is_hardware_accelerated = (codec->capabilities & AV_CODEC_CAP_HARDWARE) != 0;
        
        // Determine hardware type if applicable
        if (info.is_hardware_accelerated) {
            if (strstr(codec->name, "nvenc") || strstr(codec->name, "h264_nvenc")) {
                info.hardware_type = HardwareEncoder::NVENC;
            } else if (strstr(codec->name, "qsv") || strstr(codec->name, "h264_qsv")) {
                info.hardware_type = HardwareEncoder::QSV;
            } else if (strstr(codec->name, "amf") || strstr(codec->name, "h264_amf")) {
                info.hardware_type = HardwareEncoder::AMF;
            } else if (strstr(codec->name, "videotoolbox") || strstr(codec->name, "h264_videotoolbox")) {
                info.hardware_type = HardwareEncoder::VideoToolbox;
            }
        }
        
        codecs.push_back(std::move(info));
    }
    
    return codecs;
}

std::vector<FFmpegDiscovery::FormatInfo> FFmpegDiscovery::discover_formats() {
    std::vector<FormatInfo> formats;
    
    void* opaque = nullptr;
    const AVOutputFormat* output_format = nullptr;
    
    // Discover output formats
    while ((output_format = av_muxer_iterate(&opaque)) != nullptr) {
        FormatInfo info;
        info.name = output_format->name ? output_format->name : "";
        info.long_name = output_format->long_name ? output_format->long_name : "";
        info.extensions = output_format->extensions ? output_format->extensions : "";
        info.mime_type = output_format->mime_type ? output_format->mime_type : "";
        info.can_mux = true;
        info.can_demux = false;
        formats.push_back(std::move(info));
    }
    
    // Discover input formats
    opaque = nullptr;
    const AVInputFormat* input_format = nullptr;
    
    while ((input_format = av_demuxer_iterate(&opaque)) != nullptr) {
        FormatInfo info;
        info.name = input_format->name ? input_format->name : "";
        info.long_name = input_format->long_name ? input_format->long_name : "";
        info.extensions = input_format->extensions ? input_format->extensions : "";
        info.mime_type = input_format->mime_type ? input_format->mime_type : "";
        info.can_demux = true;
        info.can_mux = false;
        formats.push_back(std::move(info));
    }
    
    return formats;
}

std::vector<FFmpegDiscovery::HardwareCapability> FFmpegDiscovery::discover_hardware_encoders() {
    std::vector<HardwareCapability> capabilities;
    
    // Probe each hardware encoder type
    capabilities.push_back(probe_hardware_encoder(HardwareEncoder::NVENC));
    capabilities.push_back(probe_hardware_encoder(HardwareEncoder::QSV));
    capabilities.push_back(probe_hardware_encoder(HardwareEncoder::AMF));
    capabilities.push_back(probe_hardware_encoder(HardwareEncoder::VideoToolbox));
    capabilities.push_back(probe_hardware_encoder(HardwareEncoder::VAAPI));
    capabilities.push_back(probe_hardware_encoder(HardwareEncoder::VDPAU));
    
    return capabilities;
}

FFmpegDiscovery::HardwareCapability FFmpegDiscovery::probe_hardware_encoder(HardwareEncoder encoder_type) {
    HardwareCapability capability;
    capability.type = encoder_type;
    capability.is_available = false;
    
    switch (encoder_type) {
        case HardwareEncoder::NVENC:
            capability = probe_nvenc();
            break;
        case HardwareEncoder::QSV:
            capability = probe_qsv();
            break;
        case HardwareEncoder::AMF:
            capability = probe_amf();
            break;
        case HardwareEncoder::VideoToolbox:
            capability = probe_videotoolbox();
            break;
        case HardwareEncoder::VAAPI:
            capability = probe_vaapi();
            break;
        case HardwareEncoder::VDPAU:
            capability = probe_vdpau();
            break;
        default:
            capability.error_message = "Unknown hardware encoder type";
            break;
    }
    
    return capability;
}

FFmpegDiscovery::HardwareCapability FFmpegDiscovery::probe_nvenc() {
    HardwareCapability capability;
    capability.type = HardwareEncoder::NVENC;
    capability.name = "NVIDIA NVENC";
    capability.description = "NVIDIA GPU-based hardware encoding";
    
#ifdef _WIN32
    // Check for NVIDIA GPU and NVENC support on Windows
    try {
        // Try to create D3D11 device to check for NVIDIA GPU
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        
        D3D_FEATURE_LEVEL feature_levels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };
        
        HRESULT hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            feature_levels, 3, D3D11_SDK_VERSION,
            &device, nullptr, &context
        );
        
        if (SUCCEEDED(hr) && device != nullptr) {
            // Check if this is an NVIDIA GPU
            IDXGIDevice* dxgi_device = nullptr;
            hr = device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device);
            
            if (SUCCEEDED(hr) && dxgi_device != nullptr) {
                IDXGIAdapter* adapter = nullptr;
                hr = dxgi_device->GetAdapter(&adapter);
                
                if (SUCCEEDED(hr) && adapter != nullptr) {
                    DXGI_ADAPTER_DESC desc;
                    hr = adapter->GetDesc(&desc);
                    
                    if (SUCCEEDED(hr)) {
                        std::wstring vendor_name(desc.Description);
                        if (vendor_name.find(L"NVIDIA") != std::wstring::npos) {
                            capability.is_available = true;
                            capability.supported_codecs = {"h264", "hevc", "av1"};
                            capability.supported_pixel_formats = {"nv12", "p010", "p016"};
                            capability.supported_resolutions = {1920, 2560, 3840, 4096};
                            capability.supported_framerates = {30, 60, 120};
                        }
                    }
                    adapter->Release();
                }
                dxgi_device->Release();
            }
            device->Release();
            if (context) context->Release();
        }
    } catch (...) {
        capability.error_message = "Failed to probe NVIDIA GPU";
    }
#else
    // On non-Windows platforms, check if NVENC codecs are available
    if (avcodec_find_encoder_by_name("h264_nvenc") != nullptr ||
        avcodec_find_encoder_by_name("hevc_nvenc") != nullptr) {
        capability.is_available = true;
        capability.supported_codecs = {"h264", "hevc"};
    } else {
        capability.error_message = "NVENC codecs not available";
    }
#endif
    
    return capability;
}

FFmpegDiscovery::HardwareCapability FFmpegDiscovery::probe_qsv() {
    HardwareCapability capability;
    capability.type = HardwareEncoder::QSV;
    capability.name = "Intel Quick Sync Video";
    capability.description = "Intel GPU-based hardware encoding";
    
    // Check if QSV codecs are available
    if (avcodec_find_encoder_by_name("h264_qsv") != nullptr ||
        avcodec_find_encoder_by_name("hevc_qsv") != nullptr) {
        capability.is_available = true;
        capability.supported_codecs = {"h264", "hevc", "mpeg2"};
        capability.supported_pixel_formats = {"nv12", "p010"};
        capability.supported_resolutions = {1920, 2560, 3840};
        capability.supported_framerates = {30, 60};
    } else {
        capability.error_message = "QSV codecs not available";
    }
    
    return capability;
}

FFmpegDiscovery::HardwareCapability FFmpegDiscovery::probe_amf() {
    HardwareCapability capability;
    capability.type = HardwareEncoder::AMF;
    capability.name = "AMD Advanced Media Framework";
    capability.description = "AMD GPU-based hardware encoding";
    
    // Check if AMF codecs are available
    if (avcodec_find_encoder_by_name("h264_amf") != nullptr ||
        avcodec_find_encoder_by_name("hevc_amf") != nullptr) {
        capability.is_available = true;
        capability.supported_codecs = {"h264", "hevc"};
        capability.supported_pixel_formats = {"nv12", "p010"};
        capability.supported_resolutions = {1920, 2560, 3840};
        capability.supported_framerates = {30, 60};
    } else {
        capability.error_message = "AMF codecs not available";
    }
    
    return capability;
}

FFmpegDiscovery::HardwareCapability FFmpegDiscovery::probe_videotoolbox() {
    HardwareCapability capability;
    capability.type = HardwareEncoder::VideoToolbox;
    capability.name = "macOS VideoToolbox";
    capability.description = "Apple GPU-based hardware encoding";
    
#ifdef __APPLE__
    // Check if VideoToolbox is available on macOS
    if (avcodec_find_encoder_by_name("h264_videotoolbox") != nullptr ||
        avcodec_find_encoder_by_name("hevc_videotoolbox") != nullptr) {
        capability.is_available = true;
        capability.supported_codecs = {"h264", "hevc", "prores"};
        capability.supported_pixel_formats = {"nv12", "p010", "uyvy422"};
        capability.supported_resolutions = {1920, 2560, 3840, 4096};
        capability.supported_framerates = {30, 60};
    } else {
        capability.error_message = "VideoToolbox codecs not available";
    }
#else
    capability.error_message = "VideoToolbox only available on macOS";
#endif
    
    return capability;
}

FFmpegDiscovery::HardwareCapability FFmpegDiscovery::probe_vaapi() {
    HardwareCapability capability;
    capability.type = HardwareEncoder::VAAPI;
    capability.name = "Linux VAAPI";
    capability.description = "Linux Video Acceleration API";
    
#ifdef __linux__
    // Check if VAAPI is available on Linux
    if (avcodec_find_encoder_by_name("h264_vaapi") != nullptr ||
        avcodec_find_encoder_by_name("hevc_vaapi") != nullptr) {
        capability.is_available = true;
        capability.supported_codecs = {"h264", "hevc", "mpeg2"};
        capability.supported_pixel_formats = {"nv12", "p010"};
        capability.supported_resolutions = {1920, 2560, 3840};
        capability.supported_framerates = {30, 60};
    } else {
        capability.error_message = "VAAPI codecs not available";
    }
#else
    capability.error_message = "VAAPI only available on Linux";
#endif
    
    return capability;
}

FFmpegDiscovery::HardwareCapability FFmpegDiscovery::probe_vdpau() {
    HardwareCapability capability;
    capability.type = HardwareEncoder::VDPAU;
    capability.name = "Linux VDPAU";
    capability.description = "Linux Video Decode and Presentation API";
    
#ifdef __linux__
    // Check if VDPAU is available on Linux
    if (avcodec_find_decoder_by_name("h264_vdpau") != nullptr ||
        avcodec_find_decoder_by_name("hevc_vdpau") != nullptr) {
        capability.is_available = true;
        capability.supported_codecs = {"h264", "hevc", "mpeg2"};
        capability.supported_pixel_formats = {"nv12"};
        capability.supported_resolutions = {1920, 2560, 3840};
        capability.supported_framerates = {30, 60};
    } else {
        capability.error_message = "VDPAU codecs not available";
    }
#else
    capability.error_message = "VDPAU only available on Linux";
#endif
    
    return capability;
}

std::unordered_map<std::string, std::string> FFmpegDiscovery::get_build_config() {
    std::unordered_map<std::string, std::string> config;
    
    // Get FFmpeg build configuration
    const char* configuration = avutil_configuration();
    if (configuration) {
        config["configuration"] = configuration;
    }
    
    // Get license information
    const char* license = avutil_license();
    if (license) {
        config["license"] = license;
    }
    
    // Add platform-specific information
#ifdef _WIN32
    config["platform"] = "Windows";
#elif defined(__APPLE__)
    config["platform"] = "macOS";
#elif defined(__linux__)
    config["platform"] = "Linux";
#else
    config["platform"] = "Unknown";
#endif
    
    return config;
}

} // namespace capturekit
