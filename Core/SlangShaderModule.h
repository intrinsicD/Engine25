//
// Created by alex on 08.04.25. // Adjust date if needed
//

#ifndef ENGINE25_SLANGSHADERMODULE_H
#define ENGINE25_SLANGSHADERMODULE_H

#include "IModule.h"
#include <string>
#include <vector>
#include <memory> // For unique_ptr
#include <unordered_map>

// Slang API header
#include <slang/slang.h>
#include <slang/slang-com-ptr.h> // For ComPtr RAII

// Vulkan header for VkShaderModule
#include <vulkan/vulkan.h>

namespace Bcg {

    // Forward declare VulkanContext if needed by users (though CreateVkShaderModule needs it internally)
    struct VulkanContext;

    // Structure to hold compiled shader data (can be expanded)
    struct CompiledShaderData {
        std::vector<uint8_t> bytecode; // SPIR-V bytecode
        // Could add reflection data here later
    };


    class SlangShaderModule : public IModule {
    public:
        SlangShaderModule();
        ~SlangShaderModule() override; // Ensure proper cleanup

        // --- IModule Overrides ---
        bool initialize(ApplicationContext *context) override;
        void shutdown() override;

        // --- Public Shader Compilation API ---

        /**
         * @brief Compiles a Slang shader from a file.
         * @param slangFilePath Path to the .slang source file.
         * @param entryPointName The name of the entry point function (e.g., "vertexMain").
         * @param stage The shader stage (e.g., VK_SHADER_STAGE_VERTEX_BIT).
         * @param defines Optional preprocessor defines.
         * @param forceRecompile If true, ignores cached results.
         * @return A vector containing the compiled SPIR-V bytecode, or empty on failure.
         */
        std::vector<uint8_t> CompileShaderFromFile(
                const std::string& slangFilePath,
                const std::string& entryPointName,
                VkShaderStageFlagBits stage,
                const std::vector<std::pair<std::string, std::string>>& defines = {},
                bool forceRecompile = false);

        /**
         * @brief Compiles a Slang shader from a string.
         * @param sourceCode The Slang source code as a string.
         * @param entryPointName The name of the entry point function.
         * @param stage The shader stage.
         * @param defines Optional preprocessor defines.
         * @param cacheKey A unique key for caching this string compilation result.
         * @param forceRecompile If true, ignores cached results.
         * @return A vector containing the compiled SPIR-V bytecode, or empty on failure.
         */
        std::vector<uint8_t> CompileShaderFromString(
                const std::string& sourceCode,
                const std::string& entryPointName,
                VkShaderStageFlagBits stage,
                const std::vector<std::pair<std::string, std::string>>& defines = {},
                const std::string& cacheKey = "", // Requires a key for caching strings
                bool forceRecompile = false);


        /**
         * @brief Compiles a Slang shader from a file and creates a VkShaderModule.
         * @param vk_ctx Reference to the VulkanContext containing the VkDevice.
         * @param slangFilePath Path to the .slang source file.
         * @param entryPointName The name of the entry point function.
         * @param stage The shader stage.
         * @param defines Optional preprocessor defines.
         * @param forceRecompile If true, ignores cached results.
         * @return A valid VkShaderModule handle, or VK_NULL_HANDLE on failure. The caller is responsible for destroying the module.
         */
        VkShaderModule CreateVkShaderModuleFromFile(
                VulkanContext& vk_ctx, // Pass context to get VkDevice
                const std::string& slangFilePath,
                const std::string& entryPointName,
                VkShaderStageFlagBits stage,
                const std::vector<std::pair<std::string, std::string>>& defines = {},
                bool forceRecompile = false);


        // --- Optional: Caching Control ---
        void ClearCache();


    private:
        // --- Internal Helper Methods ---
        std::vector<uint8_t> compileSlang(
                const std::string& identifier, // File path or cache key
                const std::string& entryPointName,
                VkShaderStageFlagBits stage,
                slang::ICompileRequest* request, // Assumes source is loaded into request
                bool isFile); // Distinguishes between file and string source for caching


        // --- Slang Compiler State ---
        Slang::ComPtr<slang::IGlobalSession> m_globalSession;
        Slang::ComPtr<slang::ISession> m_session;

        // --- Caching ---
        // Key: Combination of file/cacheKey + entryPoint + stage + defines? (Simplified for now)
        std::string generateCacheKey(const std::string& identifier, const std::string& entryPointName, VkShaderStageFlagBits stage, const std::vector<std::pair<std::string, std::string>>& defines);
        std::unordered_map<std::string, CompiledShaderData> m_shaderCache;

        // --- Internal Vulkan Device (optional, only needed for CreateVkShaderModule) ---
        // This assumes the device doesn't change after initialization.
        // If it can, this needs updating or passing the device every time.
        VkDevice m_vkDevice = VK_NULL_HANDLE;


        // Inherited IModule methods we don't need direct implementation for in this module
        void update(float /*deltaTime*/) override {}
        void render() override {}
        void renderUI() override {}
        void connectDispatcher() override {}
        void disconnectDispatcher() override {}
    };

} // namespace Bcg

#endif //ENGINE25_SLANGSHADERMODULE_H