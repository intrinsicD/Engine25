//
// Created by alex on 08.04.25. // Adjust date if needed
//

#include "SlangShaderModule.h"
#include "ApplicationContext.h" // For getting VulkanContext pointer during init
#include "VulkanRenderer.h"     // For VulkanContext struct definition and VK_CHECK
#include "Logger.h"

#include <fstream> // For reading files (alternative to Slang's file loading)
#include <sstream> // For string manipulation

// Define VK_CHECK locally if needed
#ifndef VK_CHECK
#define VK_CHECK(result, msg)                                         \
    if (result != VK_SUCCESS) {                                       \
        LOG_ERROR(fmt::format("{}: {}", msg, static_cast<int>(result))); \
        throw std::runtime_error(msg);                                \
    }
#endif

namespace Bcg {

    // --- Helper to convert VkShaderStageFlagBits to Slang stage ---
    // *** CORRECTED RETURN TYPE and enum values ***
    SlangStage mapVkStageToSlangStage(VkShaderStageFlagBits vkStage) {
        switch (vkStage) {
            case VK_SHADER_STAGE_VERTEX_BIT:                  return SLANG_STAGE_VERTEX;
            case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:    return SLANG_STAGE_HULL;
            case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: return SLANG_STAGE_DOMAIN;
            case VK_SHADER_STAGE_GEOMETRY_BIT:                return SLANG_STAGE_GEOMETRY;
            case VK_SHADER_STAGE_FRAGMENT_BIT:                return SLANG_STAGE_FRAGMENT;
            case VK_SHADER_STAGE_COMPUTE_BIT:                 return SLANG_STAGE_COMPUTE;
            case VK_SHADER_STAGE_RAYGEN_BIT_KHR:              return SLANG_STAGE_RAY_GENERATION;
            case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:             return SLANG_STAGE_ANY_HIT;
            case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:         return SLANG_STAGE_CLOSEST_HIT;
            case VK_SHADER_STAGE_MISS_BIT_KHR:                return SLANG_STAGE_MISS;
            case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:        return SLANG_STAGE_INTERSECTION;
            case VK_SHADER_STAGE_CALLABLE_BIT_KHR:            return SLANG_STAGE_CALLABLE;
            case VK_SHADER_STAGE_TASK_BIT_EXT:                return SLANG_STAGE_AMPLIFICATION; // Or SLANG_STAGE_TASK
            case VK_SHADER_STAGE_MESH_BIT_EXT:                return SLANG_STAGE_MESH;
                // Add other mappings as needed
            default:
                LOG_ERROR(fmt::format("Unsupported VkShaderStageFlagBits: {}", static_cast<int>(vkStage)));
                return SLANG_STAGE_NONE; // Use SLANG_STAGE_NONE or another appropriate default
        }
    }

    // --- SlangShaderModule Implementation ---

    SlangShaderModule::SlangShaderModule() : IModule("SlangShaderModule", "0.1.0") {}

    SlangShaderModule::~SlangShaderModule() {
        // Shutdown should have been called, but ensure cleanup happens
        if(isInitialized()) {
            LOG_WARN("SlangShaderModule destroyed without calling shutdown() first.");
            shutdown(); // Force shutdown
        }
    }

    bool SlangShaderModule::initialize(ApplicationContext *context) {
        if (!IModule::initialize(context)) return false;

        LOG_INFO("Initializing SlangShaderModule...");

        SlangResult result = slang::createGlobalSession(m_globalSession.writeRef());
        if (SLANG_FAILED(result)) {
            LOG_FATAL("Failed to create Slang Global Session.");
            IModule::shutdown(); // Reset base class state
            return false;
        }
        LOG_INFO("Slang Global Session created.");

        // --- Create a Session ---
        slang::SessionDesc sessionDesc = {};
        // Set compilation targets
        slang::TargetDesc targetDesc = {};
        targetDesc.format = SLANG_SPIRV;          // Target SPIR-V
        targetDesc.profile = m_globalSession->findProfile("spirv_1_5"); // Or choose profile dynamically
        if (targetDesc.profile == SLANG_PROFILE_UNKNOWN) {
            LOG_WARN("Slang profile 'spirv_1_5' not found, trying 'spirv_1_3'.");
            targetDesc.profile = m_globalSession->findProfile("spirv_1_3");
        }
        if (targetDesc.profile == SLANG_PROFILE_UNKNOWN) {
            LOG_WARN("Slang profile 'spirv_1_3' not found, trying 'spirv_1_0'.");
            targetDesc.profile = m_globalSession->findProfile("spirv_1_0");
        }
        if (targetDesc.profile == SLANG_PROFILE_UNKNOWN) {
            LOG_ERROR("Could not find a suitable Slang SPIR-V profile.");
            IModule::shutdown();
            return false;
        }

        targetDesc.flags = SLANG_TARGET_FLAG_GENERATE_SPIRV_DIRECTLY; // Recommended for SPIR-V

        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;

        // Add search paths if necessary (e.g., for #include directives)
        // const char* searchPaths[] = { "path/to/shaders/include" };
        // sessionDesc.searchPaths = searchPaths;
        // sessionDesc.searchPathCount = 1;

        result = m_globalSession->createSession(sessionDesc, m_session.writeRef());
        if (SLANG_FAILED(result)) {
            LOG_FATAL("Failed to create Slang Session.");
            m_globalSession = nullptr; // Release global session
            IModule::shutdown();
            return false;
        }
        LOG_INFO("Slang Session created.");

        // Get Vulkan device handle if Vulkan is the backend
        // Delay getting device handle until CreateVkShaderModuleFromFile is called,
        // as the renderer might not be fully initialized yet.
        // The m_vkDevice member is removed as it's better to pass vk_ctx directly.


        LOG_INFO("SlangShaderModule Initialized Successfully.");
        return true;
    }

    void SlangShaderModule::shutdown() {
        LOG_INFO("Shutting down SlangShaderModule...");

        ClearCache(); // Clear any cached data

        // Slang objects are managed by ComPtr, just reset them
        m_session = nullptr;
        m_globalSession = nullptr;
        // m_vkDevice = VK_NULL_HANDLE; // Removed


        LOG_INFO("Slang resources released.");
        IModule::shutdown(); // Call base shutdown
        LOG_INFO("SlangShaderModule Shutdown Complete.");
    }

    std::string SlangShaderModule::generateCacheKey(
            const std::string& identifier,
            const std::string& entryPointName,
            VkShaderStageFlagBits stage,
            const std::vector<std::pair<std::string, std::string>>& defines)
    {
        std::stringstream ss;
        ss << identifier << "::" << entryPointName << "::" << stage;
        for(const auto& define : defines) {
            ss << "::D" << define.first << "=" << define.second;
        }
        return ss.str();
    }


    std::vector<uint8_t> SlangShaderModule::compileSlang(
            const std::string& identifier, // File path or cache key
            const std::string& entryPointName,
            VkShaderStageFlagBits stage,
            slang::ICompileRequest* request, // Request with source already loaded
            bool isFile) // Parameter 'isFile' is unused, consider removing
    {
        if (!isInitialized() || !m_session) {
            LOG_ERROR("SlangShaderModule is not initialized.");
            return {};
        }

        SlangResult result = SLANG_OK;

        // --- Find Slang Stage ---
        // *** CORRECTED TYPE ***
        SlangStage slangStage = mapVkStageToSlangStage(stage);
        if (slangStage == SLANG_STAGE_NONE) { // *** CORRECTED COMPARISON ***
            LOG_ERROR(fmt::format("Cannot map VkShaderStage {} to Slang stage.", (int)stage));
            return {};
        }

        // --- Configure Compile Request ---
        int targetIndex = request->addCodeGenTarget(SLANG_SPIRV); // Add SPIR-V target

        // Set target-specific options if needed (e.g., Vulkan version)
        // Example: Tell Slang we're targeting Vulkan 1.3 for SPIR-V generation
        // *** CORRECTED TYPE ***
        SlangTargetRequestDesc targetReqDesc = {};
        targetReqDesc.targetIndex = targetIndex; // Specify which target index this applies to
        // targetReqDesc.flags = SLANG_TARGET_REQUEST_FLAG_GENERATE_WHOLE_PROGRAM; // Might not be needed if entry points are added

        // Setting api version capabilities (optional but good practice)
        // Use findCapability to ensure it exists before setting
        if (m_globalSession->findCapability(slang::CapabilityID::Vulkan_1_3) != SLANG_CAPABILITY_UNKNOWN) {
            request->setTargetCapability(targetIndex, slang::CapabilityID::Vulkan_1_3, 1);
            LOG_TRACE("Setting Slang target capability: Vulkan 1.3");
        } else {
            LOG_WARN("Slang capability Vulkan_1_3 not found.");
        }


        // Add defines are now handled per translation unit before calling compileSlang

        int entryPointIndex = request->addEntryPoint(
                entryPointName.c_str(),
                slangStage);

        if (entryPointIndex < 0) {
            LOG_ERROR(fmt::format("Failed to find Slang entry point '{}' in {}.", entryPointName, identifier));
            const char* diagnostics = request->getDiagnosticOutput();
            if (diagnostics && strlen(diagnostics) > 0) {
                LOG_ERROR(fmt::format("Slang Diagnostics:\n{}", diagnostics));
            }
            return {};
        }

        // --- Compile ---
        LOG_INFO(fmt::format("Compiling Slang shader: {} (Entry: {}, Stage: {})", identifier, entryPointName, stage));
        result = request->compile();

        // --- Get Diagnostics ---
        const char* diagnostics = request->getDiagnosticOutput();
        if (diagnostics && strlen(diagnostics) > 0) {
            if (SLANG_FAILED(result)) {
                LOG_ERROR(fmt::format("Slang Compilation Failed for {}:\n{}", identifier, diagnostics));
            } else {
                LOG_WARN(fmt::format("Slang Compilation Warnings for {}:\n{}", identifier, diagnostics));
            }
        }

        if (SLANG_FAILED(result)) {
            return {}; // Compilation failed
        }

        // --- Get Compiled Code ---
        size_t codeSize = 0;
        // *** Use targetIndex to get code for the specific target ***
        const void* codePtr = request->getEntryPointCode(entryPointIndex, targetIndex, &codeSize);

        if (!codePtr || codeSize == 0) {
            LOG_ERROR(fmt::format("Slang compilation succeeded for {}, but failed to retrieve bytecode for target {}.", identifier, targetIndex));
            return {};
        }

        LOG_INFO(fmt::format("Slang compilation successful for {} ({} bytes).", identifier, codeSize));

        // Copy bytecode into a vector
        std::vector<uint8_t> bytecode(codeSize);
        memcpy(bytecode.data(), codePtr, codeSize);

        return bytecode;
    }


    std::vector<uint8_t> SlangShaderModule::CompileShaderFromFile(
            const std::string& slangFilePath,
            const std::string& entryPointName,
            VkShaderStageFlagBits stage,
            const std::vector<std::pair<std::string, std::string>>& defines,
            bool forceRecompile)
    {
        std::string cacheKey = generateCacheKey(slangFilePath, entryPointName, stage, defines);

        if (!forceRecompile) {
            auto cached = m_shaderCache.find(cacheKey);
            if (cached != m_shaderCache.end()) {
                LOG_TRACE(fmt::format("Returning cached shader: {}", cacheKey));
                return cached->second.bytecode;
            }
        }

        if (!m_session) {
            LOG_ERROR("Slang session is not initialized.");
            return {};
        }

        slang::ComPtr<slang::ICompileRequest> request;
        SlangResult result = m_session->createCompileRequest(request.writeRef());
        if(SLANG_FAILED(result)){
            LOG_ERROR("Failed to create Slang compile request.");
            return {};
        }

        // --- Load Source File ---
        int translationUnitIndex = request->addTranslationUnitSourceFile(slangFilePath.c_str());
        if (translationUnitIndex < 0) {
            LOG_ERROR(fmt::format("Failed to load Slang source file: {}", slangFilePath));
            const char* diagnostics = request->getDiagnosticOutput();
            if (diagnostics && strlen(diagnostics) > 0) {
                LOG_ERROR(fmt::format("Slang Diagnostics:\n{}", diagnostics));
            }
            return {};
        }

        // Set preprocessor defines for this translation unit
        for(const auto& define : defines) {
            request->addTranslationUnitPreprocessorDefine(translationUnitIndex, define.first.c_str(), define.second.c_str());
        }

        // Compile using the helper
        // *** Removed 'isFile' parameter ***
        std::vector<uint8_t> bytecode = compileSlang(slangFilePath, entryPointName, stage, request);

        // Cache the result if compilation succeeded
        if (!bytecode.empty()) {
            LOG_TRACE(fmt::format("Caching shader: {}", cacheKey));
            m_shaderCache[cacheKey] = {bytecode};
        }

        return bytecode;
    }

    std::vector<uint8_t> SlangShaderModule::CompileShaderFromString(
            const std::string& sourceCode,
            const std::string& entryPointName,
            VkShaderStageFlagBits stage,
            const std::vector<std::pair<std::string, std::string>>& defines,
            const std::string& cacheKey, // Must provide a key for string caching
            bool forceRecompile)
    {
        if (cacheKey.empty()) {
            LOG_ERROR("Cannot cache shader from string without a provided cacheKey.");
            return {};
        }

        std::string fullCacheKey = generateCacheKey(cacheKey, entryPointName, stage, defines);

        if (!forceRecompile) {
            auto cached = m_shaderCache.find(fullCacheKey);
            if (cached != m_shaderCache.end()) {
                LOG_TRACE(fmt::format("Returning cached shader: {}", fullCacheKey));
                return cached->second.bytecode;
            }
        }

        if (!m_session) {
            LOG_ERROR("Slang session is not initialized.");
            return {};
        }

        slang::ComPtr<slang::ICompileRequest> request;
        SlangResult result = m_session->createCompileRequest(request.writeRef());
        if(SLANG_FAILED(result)){
            LOG_ERROR("Failed to create Slang compile request.");
            return {};
        }

        // --- Load Source String ---
        int translationUnitIndex = request->addTranslationUnitSourceString(cacheKey.c_str(), sourceCode.c_str());
        if (translationUnitIndex < 0) {
            LOG_ERROR(fmt::format("Failed to load Slang source string (Key: {})", cacheKey));
            const char* diagnostics = request->getDiagnosticOutput();
            if (diagnostics && strlen(diagnostics) > 0) {
                LOG_ERROR(fmt::format("Slang Diagnostics:\n{}", diagnostics));
            }
            return {};
        }

        // Set preprocessor defines for this translation unit
        for(const auto& define : defines) {
            request->addTranslationUnitPreprocessorDefine(translationUnitIndex, define.first.c_str(), define.second.c_str());
        }

        // Compile using the helper
        // *** Removed 'isFile' parameter ***
        std::vector<uint8_t> bytecode = compileSlang(cacheKey, entryPointName, stage, request);

        // Cache the result if compilation succeeded
        if (!bytecode.empty()) {
            LOG_TRACE(fmt::format("Caching shader: {}", fullCacheKey));
            m_shaderCache[fullCacheKey] = {bytecode};
        }

        return bytecode;
    }


    VkShaderModule SlangShaderModule::CreateVkShaderModuleFromFile(
            VulkanContext& vk_ctx, // Get device from here
            const std::string& slangFilePath,
            const std::string& entryPointName,
            VkShaderStageFlagBits stage,
            const std::vector<std::pair<std::string, std::string>>& defines,
            bool forceRecompile)
    {
        if (vk_ctx.device == VK_NULL_HANDLE) {
            LOG_ERROR("Cannot create VkShaderModule: Invalid VkDevice handle in provided VulkanContext.");
            return VK_NULL_HANDLE;
        }

        // 1. Compile the shader to SPIR-V bytecode
        std::vector<uint8_t> bytecode = CompileShaderFromFile(slangFilePath, entryPointName, stage, defines, forceRecompile);

        if (bytecode.empty()) {
            LOG_ERROR(fmt::format("Failed to compile Slang shader {} for VkShaderModule creation.", slangFilePath));
            return VK_NULL_HANDLE;
        }

        // 2. Create VkShaderModule from bytecode
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = bytecode.size();
        // Ensure alignment for SPIR-V (uint32_t)
        if (bytecode.size() % sizeof(uint32_t) != 0) {
            LOG_ERROR(fmt::format("SPIR-V bytecode size ({}) is not a multiple of sizeof(uint32_t) for shader: {}", bytecode.size(), slangFilePath));
            return VK_NULL_HANDLE;
        }
        createInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());


        VkShaderModule shaderModule = VK_NULL_HANDLE;
        // Use the device handle directly from the passed-in vk_ctx
        VkResult result = vkCreateShaderModule(vk_ctx.device, &createInfo, nullptr, &shaderModule);

        if (result != VK_SUCCESS) {
            LOG_ERROR(fmt::format("Failed to create VkShaderModule for {}: {}", slangFilePath, static_cast<int>(result)));
            return VK_NULL_HANDLE;
        }

        LOG_INFO(fmt::format("VkShaderModule created successfully for: {}", slangFilePath));
        return shaderModule;
    }

    void SlangShaderModule::ClearCache() {
        if (!m_shaderCache.empty()) {
            LOG_INFO(fmt::format("Clearing {} cached shaders.", m_shaderCache.size()));
            m_shaderCache.clear();
        }
    }

} // namespace Bcg