//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_BACKENDDESCRIPTOR_H
#define ENGINE25_BACKENDDESCRIPTOR_H

#include <string>
#include "ConfigFile.h" // To potentially include a loading helper
#include "Logger.h"     // For logging during loading

namespace Bcg {

    // Use an enum for type safety
    enum class RendererType {
        Unknown,
        OpenGL,
        Vulkan
        // Future: DirectX, Metal, etc.
    };

    // Helper to convert string from config to enum
    inline RendererType StringToRendererType(const std::string& typeStr) {
        if (typeStr == "opengl" || typeStr == "OpenGL") return RendererType::OpenGL;
        if (typeStr == "vulkan" || typeStr == "Vulkan") return RendererType::Vulkan;
        return RendererType::Unknown;
    }

    inline std::string RendererTypeToString(RendererType type) {
        switch (type) {
            case RendererType::OpenGL: return "OpenGL";
            case RendererType::Vulkan: return "Vulkan";
            default: return "Unknown";
        }
    }

    struct BackendDescriptor {
        RendererType type = RendererType::OpenGL; // Default type
        bool vsync = true;

        // --- OpenGL Specific Settings ---
        struct OpenGLSettings {
            int majorVersion = 4; // Default Major
            int minorVersion = 6; // Default Minor
        } opengl; // Nested struct for organization

        // --- Vulkan Specific Settings ---
        struct VulkanSettings {
            bool enableValidationLayers = true;
            bool enableDebugMessenger = true; // Often tied to validation layers
        } vulkan;

        // Add settings for other backends here later...

        // --- Static Helper to Load from Config ---
        // Returns true on success, false on critical failure (e.g., type missing/unknown)
        static bool LoadFromConfig(BackendDescriptor& desc) {
            std::string rendererTypeStr = "opengl"; // Default

            try {
                rendererTypeStr = Config::get_string("renderer.type");
                desc.type = StringToRendererType(rendererTypeStr);
                if (desc.type == RendererType::Unknown) {
                    LOG_ERROR(fmt::format("Config: Unknown renderer type specified: '{}'", rendererTypeStr));
                    return false; // Indicate critical failure
                }
            } catch (const std::exception& e) {
                LOG_WARN(fmt::format("Config: Could not read 'renderer.type'. Using default '{}'. Error: {}", rendererTypeStr, e.what()));
                // Stick with the default type set above
                desc.type = StringToRendererType(rendererTypeStr);
            }

            try {
                desc.vsync = Config::get_int("renderer.vsync");
            } catch (const std::exception& e) {
                LOG_WARN(fmt::format("Config: Could not read 'renderer.vsync'. Using default {}. Error: {}", desc.vsync ? "true" : "false", e.what()));
            }

            // Load OpenGL specific settings only if type is OpenGL
            if (desc.type == RendererType::OpenGL) {
                try {
                    desc.opengl.majorVersion = Config::get_int("renderer.opengl.major_version");
                } catch (const std::exception& e) {
                    LOG_WARN(fmt::format("Config: Could not read 'renderer.opengl.major_version'. Using default {}. Error: {}", desc.opengl.majorVersion, e.what()));
                }
                try {
                    desc.opengl.minorVersion = Config::get_int("renderer.opengl.minor_version");
                } catch (const std::exception& e) {
                    LOG_WARN(fmt::format("Config: Could not read 'renderer.opengl.minor_version'. Using default {}. Error: {}", desc.opengl.minorVersion, e.what()));
                }
            }

            // Load Vulkan specific settings only if type is Vulkan
            if (desc.type == RendererType::Vulkan) {
                try {
                    desc.vulkan.enableValidationLayers = Config::get_int("renderer.vulkan.validation_layers");
                } catch (const std::exception& e) {
                    LOG_WARN(fmt::format("Config: Could not read 'renderer.vulkan.validation_layers'. Using default {}. Error: {}", desc.vulkan.enableValidationLayers ? "true" : "false", e.what()));
                }
                try {
                    desc.vulkan.enableDebugMessenger = Config::get_int("renderer.vulkan.debug_messenger");
                } catch (const std::exception& e) {
                    // Default might depend on validation layers setting
                    desc.vulkan.enableDebugMessenger = desc.vulkan.enableValidationLayers;
                    LOG_WARN(fmt::format("Config: Could not read 'renderer.vulkan.debug_messenger'. Defaulting based on validation layers ({}). Error: {}", desc.vulkan.enableDebugMessenger ? "true" : "false", e.what()));
                }
            }

            return true; // Loading succeeded (even if some values defaulted)
        }
    };

} // namespace Bcg

#endif //ENGINE25_BACKENDDESCRIPTOR_H
