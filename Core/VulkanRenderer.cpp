// VulkanRenderer.cpp
#include "VulkanRenderer.h"
#include "Logger.h"
#include "ApplicationContext.h"
#include "Platform.h" // For window access via context
// #include "MeshRenderPass.h" // Example include for a default pass

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> // For surface creation and extent query

#include <vector>
#include <memory>
#include <stdexcept>
#include <set>
#include <optional>
#include <algorithm>
#include <limits>
#include <cstring> // For strcmp

// Helper macro
#define VK_CHECK(result, msg)                                         \
    if (result != VK_SUCCESS) {                                       \
        LOG_ERROR(fmt::format("{}: {}", msg, static_cast<int>(result))); \
        throw std::runtime_error(msg);                                \
    }

namespace Bcg {

    // --- VulkanContext Definition (Now internal focus) ---
    // Keep necessary handles needed by passes or internal logic


    // --- Static Helper Function Implementations ---

    // (debugCallback remains the same as before)
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* /*pUserData*/) {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            LOG_WARN(fmt::format("Vulkan Validation Layer: {}", pCallbackData->pMessage));
        } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
            LOG_INFO(fmt::format("Vulkan Validation Layer: {}", pCallbackData->pMessage));
        } else {
            // LOG_TRACE(fmt::format("Vulkan Validation Layer: {}", pCallbackData->pMessage)); // Verbose
        }
        return VK_FALSE; // Application should not be aborted
    }


    void VulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | // Optional: For detailed info
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |    // Optional: General info
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;
    }

    VkResult VulkanRenderer::CreateDebugUtilsMessengerEXT(VkInstance inst, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(inst, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(inst, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void VulkanRenderer::DestroyDebugUtilsMessengerEXT(VkInstance inst, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(inst, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(inst, debugMessenger, pAllocator);
        }
    }

    std::vector<const char*> VulkanRenderer::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (m_enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        return extensions;
    }

    bool VulkanRenderer::checkValidationLayerSupport(const std::vector<const char*>& layers) {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        LOG_INFO("Available Vulkan Validation Layers:");
        for(const auto& layerProp : availableLayers) { LOG_INFO(fmt::format("  - {}", layerProp.layerName)); }

        for (const char* layerName : layers) {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) {
                LOG_ERROR(fmt::format("Validation layer requested but not available: {}", layerName));
                return false;
            }
        }
        LOG_INFO("All requested validation layers are available.");
        return true;
    }


    VulkanRenderer::QueueFamilyIndices VulkanRenderer::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            // Check for Graphics Bit
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
                LOG_TRACE(fmt::format("Found graphics queue family at index: {}", i));
            }

            // Check for Presentation Support
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
                LOG_TRACE(fmt::format("Found present queue family at index: {}", i));
            }

            if (indices.isComplete()) {
                break;
            }
            i++;
        }
        return indices;
    }

    bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        LOG_INFO("Required device extensions:");
        for(const auto& req_ext : deviceExtensions) { LOG_INFO(fmt::format("  - {}", req_ext)); }


        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        LOG_INFO("Available device extensions:");
        for(const auto& avail_ext : availableExtensions) { LOG_INFO(fmt::format("  - {}", avail_ext.extensionName)); }


        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        if(!requiredExtensions.empty()) {
            LOG_ERROR("Missing required device extensions:");
            for(const auto& missing : requiredExtensions) { LOG_ERROR(fmt::format("  - {}", missing)); }
        }

        return requiredExtensions.empty();
    }

    VulkanRenderer::SwapChainSupportDetails VulkanRenderer::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }
        return details;
    }

    VkSurfaceFormatKHR VulkanRenderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                LOG_INFO("Chosen Swapchain Format: B8G8R8A8_SRGB, SRGB_NONLINEAR");
                return availableFormat;
            }
        }
        // Fallback to the first available format
        LOG_INFO(fmt::format("Chosen Swapchain Format (Fallback): Format={}, ColorSpace={}",
                             static_cast<int>(availableFormats[0].format),
                             static_cast<int>(availableFormats[0].colorSpace)));
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanRenderer::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        // Prefer Mailbox for lower latency without tearing (if available)
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                LOG_INFO("Chosen Swapchain Present Mode: Mailbox (Low Latency)");
                return availablePresentMode;
            }
        }
        // FIFO is guaranteed to be available (VSync)
        LOG_INFO("Chosen Swapchain Present Mode: Fifo (VSync)");
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    // Needs access to window handle via ApplicationContext
    VkExtent2D VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (!m_app_context || !m_app_context->windowPtr) {
            throw std::runtime_error("Window pointer not available in context for choosing swap extent!");
        }

        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            LOG_INFO(fmt::format("Swapchain Extent Fixed by Vulkan: {}x{}", capabilities.currentExtent.width, capabilities.currentExtent.height));
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(m_app_context->windowPtr, &width, &height); // Use context's window

            VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
            };

            // Clamp values to the allowed range
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            LOG_INFO(fmt::format("Swapchain Extent Chosen (from window framebuffer): {}x{}", actualExtent.width, actualExtent.height));
            return actualExtent;
        }
    }


    // --- VulkanRenderer Member Function Implementations ---

    VulkanRenderer::VulkanRenderer() : IModule("VulkanRenderer", "0.1.0"), m_vkContext(std::make_unique<VulkanContext>()) {}

    VulkanRenderer::~VulkanRenderer() {
        // Ensure shutdown is called if not already done
        if (isInitialized()) {
            LOG_WARN("VulkanRenderer destructor called while still initialized. Forcing shutdown.");
            shutdown();
        }
        // vkContext unique_ptr will automatically delete the context here
    }

    bool VulkanRenderer::initialize(ApplicationContext *context) {
        LOG_INFO("Initializing Vulkan Renderer...");
        if (!IModule::initialize(context)) { // Call base class initialize
            LOG_ERROR("Base module initialization failed.");
            return false;
        }
        if (!m_app_context || !m_app_context->windowPtr) {
            LOG_ERROR("ApplicationContext or Window handle is null!");
            m_initialized = false; // Reset initialized flag
            return false;
        }

        // Store VulkanContext in the scene context map for potential external access (careful!)
        // Or just keep it private within the renderer. Decide based on needs.
        // For now, let's assume VulkanContext is primarily internal.
        // context->scene->ctx().emplace<VulkanContext>(*m_vkContext); // If sharing needed

        try {
            if (!createInstance()) return false;
            setupDebugMessenger();
            if (!createSurface()) return false;
            if (!pickPhysicalDevice()) return false;
            if (!createLogicalDevice()) return false;
            if (!createCommandPool()) return false;
            CreateSwapChain(); // Creates swapchain object and images
            CreateSwapchainImageViews(); // Creates views for the images
            if (!allocateCommandBuffers()) return false; // Allocate main command buffers
            if (!createSyncObjects()) return false; // Create semaphores/fences

            // Initialize registered RenderPasses AFTER core Vulkan setup
            LOG_INFO(fmt::format("Initializing {} registered render passes...", m_render_passes.size()));
            for (auto& pass : m_render_passes) {
                if (!pass->Initialize(*m_vkContext, m_app_context)) {
                    LOG_ERROR(fmt::format("Failed to initialize RenderPass: {}", pass->getName()));
                    shutdown(); // Cleanup partially initialized state
                    return false;
                }
                // Also call OnSwapChainCreate for the initial swapchain setup
                pass->OnSwapChainCreate(*m_vkContext);
            }
            LOG_INFO("Render passes initialized.");


        } catch (const std::runtime_error& e) {
            LOG_ERROR(fmt::format("Vulkan initialization failed: {}", e.what()));
            shutdown(); // Clean up whatever was created
            m_initialized = false; // Ensure flag is false on failure
            return false;
        }

        LOG_INFO("Vulkan Renderer Initialized Successfully.");
        // m_initialized is set by IModule::initialize
        return true;
    }

    void VulkanRenderer::shutdown() {
        LOG_INFO("Shutting down Vulkan Renderer...");
        if (!isInitialized()) {
            LOG_WARN("VulkanRenderer::shutdown() called but already shut down or never initialized.");
            return;
        }

        if (m_vkContext && m_vkContext->device) {
            vkDeviceWaitIdle(m_vkContext->device); // Wait for GPU to finish before cleanup
            LOG_INFO("Vulkan device wait idle completed.");
        } else {
            LOG_WARN("Cannot wait for device idle: Logical device handle is null.");
        }

        // 1. Cleanup Swapchain (calls OnSwapChainDestroy on passes, destroys views/swapchain obj)
        CleanupSwapChain();

        // 2. Shutdown Render Passes (calls pass->Shutdown)
        LOG_INFO("Shutting down render passes...");
        for (auto& pass : m_render_passes) {
            if (pass) { // Check if pointer is valid
                // OnSwapChainDestroy was already called by CleanupSwapChain
                pass->Shutdown(*m_vkContext);
            }
        }
        m_render_passes.clear(); // Clear the list
        LOG_INFO("Render passes shut down.");

        // 3. Cleanup Renderer's own resources (sync objects, command pool/buffers)
        cleanupSyncObjects();
        cleanupCommands(); // Pool and Buffers

        // 4. Cleanup Core Vulkan Objects (Device, Surface, Debugger, Instance)
        cleanupCore();

        // Reset context unique_ptr AFTER all resources using it are destroyed
        m_vkContext.reset(new VulkanContext()); // Reset to a fresh context or nullptr if preferred

        // Call base class shutdown LAST
        IModule::shutdown();

        LOG_INFO("Vulkan Renderer Shutdown Complete.");
    }

    void VulkanRenderer::update(float /*deltaTime*/) {
        // Usually not much needed here for the renderer itself,
        // but could be used for internal state updates if required.
        // RenderPasses might handle their own updates if necessary.
    }

    void VulkanRenderer::render() {
        if (!isInitialized() || !m_vkContext || !m_vkContext->device || !m_vkContext->swapChain) {
            // LOG_TRACE("Renderer not ready, skipping frame."); // Can be noisy
            return; // Not ready
        }

        // --- Wait for the previous frame to finish ---
        VkFence currentFence = m_vkContext->inFlightFences[m_currentFrame];
        vkWaitForFences(m_vkContext->device, 1, &currentFence, VK_TRUE, UINT64_MAX);
        // Note: No vkResetFences here, reset happens after successful vkQueueSubmit

        // --- Acquire an image from the swap chain ---
        uint32_t imageIndex;
        VkSemaphore imageAvailableSemaphore = m_vkContext->imageAvailableSemaphores[m_currentFrame];
        VkResult result = vkAcquireNextImageKHR(m_vkContext->device, m_vkContext->swapChain,
                                                UINT64_MAX, // Timeout (disabled)
                                                imageAvailableSemaphore, // Semaphore to signal
                                                VK_NULL_HANDLE, // Fence to signal (optional)
                                                &imageIndex);

        // --- Handle Swapchain Out-of-Date / Suboptimal ---
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            LOG_WARN("Swapchain out of date during vkAcquireNextImageKHR. Recreating...");
            RecreateSwapChain();
            return; // Skip rest of the frame, vkAcquireNextImageKHR will be called again next time
        } else if (result == VK_SUBOPTIMAL_KHR) {
            // Still usable, but trigger recreation after presentation
            LOG_WARN("Swapchain suboptimal during vkAcquireNextImageKHR. Will recreate after present.");
            m_framebufferResized = true; // Use the resize flag to trigger recreation
        } else if (result != VK_SUCCESS) {
            // Other errors
            VK_CHECK(result, "Failed to acquire swap chain image!"); // Throws exception
        }

        // --- Reset the fence *only* after successfully acquiring an image and knowing we'll submit ---
        vkResetFences(m_vkContext->device, 1, &currentFence);


        // --- Record Command Buffer ---
        VkCommandBuffer commandBuffer = m_vkContext->commandBuffers[m_currentFrame];
        vkResetCommandBuffer(commandBuffer, 0); // Reset before recording
        RecordCommandBuffer(imageIndex); // Fills the command buffer via RenderPasses

        // --- Submit Command Buffer ---
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;   // Wait for image to be available
        submitInfo.pWaitDstStageMask = waitStages;    // Stage(s) to wait at

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer; // Command buffer to execute

        VkSemaphore signalSemaphores[] = {m_vkContext->renderFinishedSemaphores[m_currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores; // Signal when rendering finishes

        VK_CHECK(vkQueueSubmit(m_vkContext->graphicsQueue, 1, &submitInfo, currentFence), // Use the frame's fence
                 "Failed to submit draw command buffer!");

        // --- Present the image ---
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores; // Wait for rendering to finish

        VkSwapchainKHR swapChains[] = {m_vkContext->swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex; // Index of the image to present
        presentInfo.pResults = nullptr; // Optional: Check results for each swapchain

        result = vkQueuePresentKHR(m_vkContext->presentQueue, &presentInfo);

        // --- Handle Post-Present Swapchain Status ---
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                LOG_WARN("Swapchain out of date after vkQueuePresentKHR. Recreating...");
            } else if (result == VK_SUBOPTIMAL_KHR) {
                LOG_WARN("Swapchain suboptimal after vkQueuePresentKHR. Recreating...");
            } else { // m_framebufferResized was true
                LOG_INFO("Framebuffer resize detected. Recreating swapchain...");
            }
            m_framebufferResized = false; // Reset flag
            RecreateSwapChain();
            // Don't return early here, the frame was successfully presented (though maybe suboptimal)
        } else if (result != VK_SUCCESS) {
            VK_CHECK(result, "Failed to present swap chain image!"); // Throws exception
        }

        // --- Advance to the next frame index ---
        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::renderUI() {
        // This function is now less about *generic* UI and more about
        // integrating a specific UI system like ImGui, if used.
        // If ImGui is a RenderPass, this function might do nothing or
        // trigger specific ImGui Vulkan backend rendering steps if needed
        // outside the main pass execution.

        // Example: If ImGui is managed separately (not recommended for complex scenes)
        // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_vkContext->commandBuffers[m_currentFrame]);

        // For now, assume UI rendering is handled within a dedicated RenderPass called during RecordCommandBuffer.
        // LOG_TRACE("VulkanRenderer::renderUI() called - UI assumed handled by a RenderPass.");
    }

    void VulkanRenderer::AddRenderPass(std::shared_ptr<RenderPass> pass) {
        if (!pass) return;
        LOG_INFO(fmt::format("Adding RenderPass: {}", pass->getName()));
        m_render_passes.push_back(std::move(pass));

        // If the renderer is already initialized, initialize the new pass immediately
        if (isInitialized() && m_vkContext && m_vkContext->device) {
            LOG_INFO(fmt::format("Initializing newly added RenderPass: {}", m_render_passes.back()->getName()));
            if (!m_render_passes.back()->Initialize(*m_vkContext, m_app_context)) {
                LOG_ERROR(fmt::format("Failed to initialize dynamically added RenderPass: {}. Removing.", m_render_passes.back()->getName()));
                m_render_passes.pop_back();
                // Consider throwing or handling the error more robustly
            } else {
                // Also need to create swapchain-dependent resources
                m_render_passes.back()->OnSwapChainCreate(*m_vkContext);
            }
        }
    }


    void VulkanRenderer::FramebufferResizeCallback() {
        // Called by the platform (e.g., GLFW callback) when the window is resized
        LOG_INFO("Framebuffer resize event received, flagging for swapchain recreation.");
        m_framebufferResized = true;
    }

    VulkanContext* VulkanRenderer::GetVulkanContext() {
        return m_vkContext.get();
    }


    // --- Private Vulkan Setup Methods ---

    bool VulkanRenderer::createInstance() {
        if (m_enableValidationLayers && !checkValidationLayerSupport(m_validationLayers)) {
            LOG_ERROR("Required validation layers not available!");
            // Option: Proceed without validation layers? (Set m_enableValidationLayers = false)
            // For now, treat as fatal error.
            return false;
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = m_app_context->windowDesc.title.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Bcg::Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3; // Request Vulkan 1.3

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        LOG_INFO("Required Vulkan Instance Extensions:");
        for(const auto& ext : extensions) { LOG_INFO(fmt::format("  - {}", ext)); }


        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{}; // Declare here for scope
        if (m_enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
            createInfo.ppEnabledLayerNames = m_validationLayers.data();
            LOG_INFO("Validation layers enabled.");
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo; // Chain debug messenger create info for instance creation
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
            createInfo.pNext = nullptr;
            LOG_INFO("Validation layers disabled.");
        }

        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_vkContext->instance), "Failed to create Vulkan instance!");
        LOG_INFO("Vulkan instance created successfully.");
        return true;
    }

    void VulkanRenderer::setupDebugMessenger() {
        if (!m_enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);
        VK_CHECK(CreateDebugUtilsMessengerEXT(m_vkContext->instance, &createInfo, nullptr, &m_vkContext->debugMessenger),
                 "Failed to set up debug messenger!");
        LOG_INFO("Vulkan debug messenger set up successfully.");
    }

    bool VulkanRenderer::createSurface() {
        if (!m_app_context || !m_app_context->windowPtr) {
            LOG_ERROR("Cannot create surface: Window pointer is null in context.");
            return false;
        }
        // Platform-specific surface creation (using GLFW here)
        VK_CHECK(glfwCreateWindowSurface(m_vkContext->instance, m_app_context->windowPtr, nullptr, &m_vkContext->surface),
                 "Failed to create window surface using GLFW!");
        LOG_INFO("Vulkan surface created successfully.");
        return true;
    }

    bool VulkanRenderer::pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_vkContext->instance, &deviceCount, nullptr);
        if (deviceCount == 0) { LOG_ERROR("Failed to find GPUs with Vulkan support!"); return false; }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_vkContext->instance, &deviceCount, devices.data());
        LOG_INFO(fmt::format("Found {} Vulkan-capable physical device(s):", deviceCount));

        VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
        int bestScore = -1;

        for (const auto& device : devices) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);
            LOG_INFO(fmt::format("Evaluating Device: {}", properties.deviceName));
            if (isDeviceSuitable(device)) {
                int currentScore = 0;
                // Prioritize Discrete GPUs
                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    currentScore += 1000;
                    LOG_INFO("  + Score: 1000 (Discrete GPU)");
                } else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
                    currentScore += 100; // Prefer integrated over others if discrete not suitable
                    LOG_INFO("  + Score: 100 (Integrated GPU)");
                }
                // Add other scoring factors if needed (e.g., memory size)

                if (currentScore > bestScore) {
                    bestScore = currentScore;
                    bestDevice = device;
                }
            } else {
                LOG_INFO("  - Device not suitable.");
            }
        }

        if (bestDevice != VK_NULL_HANDLE) {
            m_vkContext->physicalDevice = bestDevice;
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(bestDevice, &properties);
            LOG_INFO(fmt::format("Selected Physical Device: {}", properties.deviceName));
            // Store queue family indices once device is picked
            m_vkContext->queueFamilyIndices = findQueueFamilies(m_vkContext->physicalDevice, m_vkContext->surface);
            return true;
        } else {
            LOG_ERROR("Failed to find a suitable GPU!");
            return false;
        }
    }

    // Helper for pickPhysicalDevice
    bool VulkanRenderer::isDeviceSuitable(VkPhysicalDevice device) {
        // 1. Check Queue Families
        QueueFamilyIndices indices = findQueueFamilies(device, m_vkContext->surface);
        if (!indices.isComplete()) {
            LOG_TRACE("  - Reason: Incomplete queue families.");
            return false;
        }

        // 2. Check Required Extensions (e.g., Swapchain)
        if (!checkDeviceExtensionSupport(device)) {
            LOG_TRACE("  - Reason: Missing required device extensions.");
            return false;
        }

        // 3. Check Swap Chain Adequacy
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, m_vkContext->surface);
        bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        if (!swapChainAdequate) {
            LOG_TRACE("  - Reason: Swapchain not adequate (no formats or present modes).");
            return false;
        }

        // 4. Check Required Features (if any)
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
        // Example: Check for sampler anisotropy
        // if (!supportedFeatures.samplerAnisotropy) {
        //     LOG_TRACE("  - Reason: Sampler Anisotropy not supported.");
        //     return false;
        // }

        LOG_INFO("  - Device is suitable.");
        return true;
    }


    bool VulkanRenderer::createLogicalDevice() {
        QueueFamilyIndices indices = m_vkContext->queueFamilyIndices;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        // Use a set to handle cases where graphics and present families are the same
        std::set<uint32_t> uniqueQueueFamilies = {
                indices.graphicsFamily.value(),
                indices.presentFamily.value()
        };

        float queuePriority = 1.0f; // Default priority
        for (uint32_t queueFamilyIndex : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
            queueCreateInfo.queueCount = 1; // Create one queue from this family
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
            LOG_INFO(fmt::format("Requesting queue from family index: {}", queueFamilyIndex));
        }

        // Specify required device features
        VkPhysicalDeviceFeatures deviceFeatures{};
        // Example: Enable anisotropy if available and desired
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(m_vkContext->physicalDevice, &supportedFeatures);
        if (supportedFeatures.samplerAnisotropy) {
            deviceFeatures.samplerAnisotropy = VK_TRUE;
            LOG_INFO("Enabling device feature: Sampler Anisotropy");
        } else {
            LOG_WARN("Device feature 'Sampler Anisotropy' not supported by selected physical device.");
        }
        // Enable other features as needed...


        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures; // Specify enabled features

        // Enable required device extensions (Swapchain is essential)
        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        LOG_INFO("Enabling device extensions:");
        for(const auto& ext : deviceExtensions) { LOG_INFO(fmt::format("  - {}", ext)); }


        // Enable validation layers (consistent with instance creation)
        if (m_enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
            createInfo.ppEnabledLayerNames = m_validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        VK_CHECK(vkCreateDevice(m_vkContext->physicalDevice, &createInfo, nullptr, &m_vkContext->device),
                 "Failed to create logical device!");
        LOG_INFO("Logical device created successfully.");

        // Retrieve queue handles
        vkGetDeviceQueue(m_vkContext->device, indices.graphicsFamily.value(), 0, &m_vkContext->graphicsQueue);
        vkGetDeviceQueue(m_vkContext->device, indices.presentFamily.value(), 0, &m_vkContext->presentQueue);
        LOG_INFO("Graphics and Present queue handles obtained.");

        return true;
    }

    bool VulkanRenderer::createCommandPool() {
        QueueFamilyIndices indices = m_vkContext->queueFamilyIndices;

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allow resetting individual command buffers
        poolInfo.queueFamilyIndex = indices.graphicsFamily.value(); // Pool for graphics commands

        VK_CHECK(vkCreateCommandPool(m_vkContext->device, &poolInfo, nullptr, &m_vkContext->commandPool),
                 "Failed to create command pool!");
        LOG_INFO("Command pool created successfully.");
        return true;
    }

    bool VulkanRenderer::allocateCommandBuffers() {
        m_vkContext->commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_vkContext->commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Main command buffers
        allocInfo.commandBufferCount = (uint32_t)m_vkContext->commandBuffers.size();

        VK_CHECK(vkAllocateCommandBuffers(m_vkContext->device, &allocInfo, m_vkContext->commandBuffers.data()),
                 "Failed to allocate command buffers!");
        LOG_INFO(fmt::format("Allocated {} primary command buffer(s).", m_vkContext->commandBuffers.size()));
        return true;
    }


    bool VulkanRenderer::createSyncObjects() {
        m_vkContext->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_vkContext->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_vkContext->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start fences signaled (so first frame doesn't wait)

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            VkResult res1 = vkCreateSemaphore(m_vkContext->device, &semaphoreInfo, nullptr, &m_vkContext->imageAvailableSemaphores[i]);
            VkResult res2 = vkCreateSemaphore(m_vkContext->device, &semaphoreInfo, nullptr, &m_vkContext->renderFinishedSemaphores[i]);
            VkResult res3 = vkCreateFence(m_vkContext->device, &fenceInfo, nullptr, &m_vkContext->inFlightFences[i]);

            if (res1 != VK_SUCCESS || res2 != VK_SUCCESS || res3 != VK_SUCCESS) {
                LOG_ERROR(fmt::format("Failed to create synchronization objects for frame {}!", i));
                // Cleanup already created objects for this frame before failing
                // (More robust cleanup needed in production)
                cleanupSyncObjects(); // Clean everything created so far
                return false;
            }
        }
        LOG_INFO(fmt::format("Created synchronization objects (semaphores/fences) for {} frame(s) in flight.", MAX_FRAMES_IN_FLIGHT));
        return true;
    }


    // --- Swapchain Management ---

    void VulkanRenderer::CreateSwapChain() {
        LOG_INFO("Creating Swapchain...");
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_vkContext->physicalDevice, m_vkContext->surface);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities); // Uses context window ptr

        // Determine number of images in the swap chain
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1; // Request one more than minimum
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount; // Clamp to maximum if it exists
        }
        LOG_INFO(fmt::format("Requesting {} swapchain images (min: {}, max: {}).",
                             imageCount,
                             swapChainSupport.capabilities.minImageCount,
                             swapChainSupport.capabilities.maxImageCount == 0 ? "unlimited" : std::to_string(swapChainSupport.capabilities.maxImageCount)));

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_vkContext->surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1; // Non-stereoscopic rendering
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // Use images as color attachments

        // Handle image sharing between queue families
        QueueFamilyIndices indices = m_vkContext->queueFamilyIndices;
        uint32_t queueFamilyIndicesArray[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            // Image needs to be shared between graphics and present queues
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
            LOG_INFO("Swapchain Image Sharing Mode: Concurrent (Graphics != Present)");
        } else {
            // Image owned exclusively by one queue family
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
            LOG_INFO("Swapchain Image Sharing Mode: Exclusive (Graphics == Present)");
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform; // Use current transform
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // No blending with window system
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE; // Allow clipping of obscured pixels
        createInfo.oldSwapchain = VK_NULL_HANDLE; // No old swapchain during initial creation

        VK_CHECK(vkCreateSwapchainKHR(m_vkContext->device, &createInfo, nullptr, &m_vkContext->swapChain),
                 "Failed to create swap chain!");

        // Retrieve swap chain images
        vkGetSwapchainImagesKHR(m_vkContext->device, m_vkContext->swapChain, &imageCount, nullptr); // Get actual count
        m_vkContext->swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_vkContext->device, m_vkContext->swapChain, &imageCount, m_vkContext->swapChainImages.data());
        LOG_INFO(fmt::format("Retrieved {} swapchain images.", imageCount));

        // Store format and extent for later use
        m_vkContext->swapChainImageFormat = surfaceFormat.format;
        m_vkContext->swapChainExtent = extent;
        LOG_INFO("Swapchain created successfully.");
    }

    void VulkanRenderer::CreateSwapchainImageViews() {
        LOG_INFO("Creating Swapchain Image Views...");
        m_vkContext->swapChainImageViews.resize(m_vkContext->swapChainImages.size());

        for (size_t i = 0; i < m_vkContext->swapChainImages.size(); ++i) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_vkContext->swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // Standard 2D image view
            createInfo.format = m_vkContext->swapChainImageFormat; // Use the swapchain's format

            // Default component mapping (RGBA -> RGBA)
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            // Image subresource range (what part of the image the view accesses)
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // Color part
            createInfo.subresourceRange.baseMipLevel = 0; // Start at mip level 0
            createInfo.subresourceRange.levelCount = 1; // One mip level
            createInfo.subresourceRange.baseArrayLayer = 0; // Start at array layer 0
            createInfo.subresourceRange.layerCount = 1; // One array layer

            VK_CHECK(vkCreateImageView(m_vkContext->device, &createInfo, nullptr, &m_vkContext->swapChainImageViews[i]),
                     fmt::format("Failed to create image view {}!", i));
        }
        LOG_INFO(fmt::format("Created {} swapchain image views successfully.", m_vkContext->swapChainImageViews.size()));
    }

    void VulkanRenderer::CleanupSwapChain() {
        LOG_INFO("Starting Swapchain Cleanup...");
        if (!m_vkContext || !m_vkContext->device) {
            LOG_WARN("Context or Device handle null during swapchain cleanup. Skipping.");
            return;
        }
        // Don't wait idle here if called during resize, wait happens in RecreateSwapChain
        // vkDeviceWaitIdle(m_vkContext->device); // Wait only if called during full shutdown?

        // 1. Notify Render Passes to destroy their swapchain-dependent resources FIRST
        LOG_INFO("Notifying render passes of swapchain destruction...");
        for (auto& pass : m_render_passes) {
            if (pass) { // Check if pointer is valid
                pass->OnSwapChainDestroy(*m_vkContext);
            }
        }
        LOG_INFO("Render passes notified.");

        // 2. Destroy Swapchain Image Views
        LOG_INFO("Destroying image views...");
        for (auto imageView : m_vkContext->swapChainImageViews) {
            if (imageView) vkDestroyImageView(m_vkContext->device, imageView, nullptr);
        }
        m_vkContext->swapChainImageViews.clear();
        m_vkContext->swapChainImages.clear(); // Images are owned by swapchain, just clear vector
        LOG_INFO("Image Views destroyed.");

        // 3. Destroy Swapchain Object
        if (m_vkContext->swapChain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(m_vkContext->device, m_vkContext->swapChain, nullptr);
            m_vkContext->swapChain = VK_NULL_HANDLE;
            LOG_INFO("Swapchain KHR object destroyed.");
        } else {
            LOG_INFO("Swapchain KHR object was already null.");
        }
        LOG_INFO("Swapchain cleanup finished.");
    }


    void VulkanRenderer::RecreateSwapChain() {
        LOG_INFO("Recreating Swapchain...");
        if (!m_app_context || !m_vkContext || !m_vkContext->device) {
            LOG_ERROR("Cannot recreate swapchain: Invalid context or device.");
            // This is likely a fatal error state
            throw std::runtime_error("Cannot recreate swapchain due to invalid state.");
            // return; // Or return if trying to recover gracefully (less likely here)
        }

        // Handle minimized window case (width/height = 0)
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_app_context->windowPtr, &width, &height);
        while (width == 0 || height == 0) {
            LOG_INFO("Window minimized, waiting for restore to recreate swapchain...");
            glfwWaitEvents(); // Process events while waiting
            glfwGetFramebufferSize(m_app_context->windowPtr, &width, &height);
        }
        LOG_INFO("Window restored, proceeding with swapchain recreation.");

        // --- Wait for current operations to finish ---
        vkDeviceWaitIdle(m_vkContext->device);
        LOG_INFO("Device idle. Starting recreation process.");

        // --- Cleanup old swapchain resources ---
        CleanupSwapChain(); // Calls OnSwapChainDestroy on passes, destroys views/swapchain obj

        // --- Recreate swapchain components ---
        try {
            CreateSwapChain();           // Recreate swapchain object and get images
            CreateSwapchainImageViews(); // Recreate image views

            // --- Notify Render Passes AFTER new swapchain views are ready ---
            LOG_INFO("Notifying render passes of new swapchain creation...");
            for (auto& pass : m_render_passes) {
                if (pass) {
                    pass->OnSwapChainCreate(*m_vkContext); // Passes recreate framebuffers/pipelines etc.
                }
            }
            LOG_INFO("Render passes notified of new swapchain.");

        } catch (const std::runtime_error& e) {
            LOG_ERROR(fmt::format("Failed during swapchain recreation steps: {}", e.what()));
            // If creation fails, the application is likely in an unrecoverable state
            throw; // Rethrow to signal critical failure
        }
        LOG_INFO("Swapchain recreated successfully.");
    }

    // --- Resource Cleanup Methods ---
    void VulkanRenderer::cleanupSyncObjects() {
        LOG_INFO("Destroying synchronization objects...");
        if (m_vkContext && m_vkContext->device) {
            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
                if(m_vkContext->renderFinishedSemaphores.size() > i && m_vkContext->renderFinishedSemaphores[i])
                    vkDestroySemaphore(m_vkContext->device, m_vkContext->renderFinishedSemaphores[i], nullptr);
                if(m_vkContext->imageAvailableSemaphores.size() > i && m_vkContext->imageAvailableSemaphores[i])
                    vkDestroySemaphore(m_vkContext->device, m_vkContext->imageAvailableSemaphores[i], nullptr);
                if(m_vkContext->inFlightFences.size() > i && m_vkContext->inFlightFences[i])
                    vkDestroyFence(m_vkContext->device, m_vkContext->inFlightFences[i], nullptr);
            }
            LOG_INFO("Synchronization objects destroyed.");
        } else {
            LOG_WARN("Cannot destroy sync objects: Device handle is null.");
        }
        // Clear vectors even if device was null
        if(m_vkContext) {
            m_vkContext->renderFinishedSemaphores.clear();
            m_vkContext->imageAvailableSemaphores.clear();
            m_vkContext->inFlightFences.clear();
        }
    }

    void VulkanRenderer::cleanupCommands() {
        LOG_INFO("Destroying command pool and buffers...");
        if (m_vkContext && m_vkContext->device) {
            // Command buffers are implicitly freed when the pool is destroyed
            // No need to call vkFreeCommandBuffers explicitly unless desired for specific reasons
            if(m_vkContext->commandPool) {
                vkDestroyCommandPool(m_vkContext->device, m_vkContext->commandPool, nullptr);
                m_vkContext->commandPool = VK_NULL_HANDLE;
                LOG_INFO("Command pool destroyed.");
            } else {
                LOG_INFO("Command pool was already null.");
            }
        } else {
            LOG_WARN("Cannot destroy command pool: Device handle is null.");
        }
        // Clear vector even if device was null
        if(m_vkContext) {
            m_vkContext->commandBuffers.clear();
        }
    }


    void VulkanRenderer::cleanupCore() {
        LOG_INFO("Destroying core Vulkan objects...");

        // Destroy in reverse order of creation

        // Logical Device
        if (m_vkContext && m_vkContext->device) {
            vkDestroyDevice(m_vkContext->device, nullptr);
            m_vkContext->device = VK_NULL_HANDLE;
            LOG_INFO("Logical device destroyed.");
        } else {
            LOG_INFO("Logical device was already null.");
        }

        // Surface (requires instance)
        if (m_vkContext && m_vkContext->instance && m_vkContext->surface) {
            vkDestroySurfaceKHR(m_vkContext->instance, m_vkContext->surface, nullptr);
            m_vkContext->surface = VK_NULL_HANDLE;
            LOG_INFO("Surface destroyed.");
        } else {
            LOG_INFO("Surface was already null or instance missing.");
        }


        // Debug Messenger (requires instance)
        if (m_enableValidationLayers && m_vkContext && m_vkContext->instance && m_vkContext->debugMessenger) {
            DestroyDebugUtilsMessengerEXT(m_vkContext->instance, m_vkContext->debugMessenger, nullptr);
            m_vkContext->debugMessenger = VK_NULL_HANDLE;
            LOG_INFO("Debug messenger destroyed.");
        } else {
            LOG_INFO("Debug messenger was already null/disabled or instance missing.");
        }

        // Instance
        if (m_vkContext && m_vkContext->instance) {
            vkDestroyInstance(m_vkContext->instance, nullptr);
            m_vkContext->instance = VK_NULL_HANDLE;
            LOG_INFO("Instance destroyed.");
        } else {
            LOG_INFO("Instance was already null.");
        }

        LOG_INFO("Core Vulkan object cleanup complete.");
    }


    // --- Frame Rendering Logic ---

    void VulkanRenderer::RecordCommandBuffer(uint32_t imageIndex) {
        VkCommandBuffer commandBuffer = m_vkContext->commandBuffers[m_currentFrame]; // Get the command buffer for the current frame in flight

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional: VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, etc.
        beginInfo.pInheritanceInfo = nullptr; // Optional

        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo),
                 "Failed to begin recording command buffer!");

        // --- Iterate through Render Passes ---
        // LOG_TRACE(fmt::format("Recording command buffer for frame {} using image index {}", m_currentFrame, imageIndex));
        for (auto& pass : m_render_passes) {
            if (pass) {
                // LOG_TRACE(fmt::format("Executing render pass: {}", pass->getName()));
                try {
                    pass->Execute(*m_vkContext, commandBuffer, imageIndex);
                } catch (const std::exception& e) {
                    LOG_ERROR(fmt::format("Exception during RenderPass '{}' execution: {}", pass->getName(), e.what()));
                    // Decide how to handle pass error (skip rest? stop frame?)
                    // For now, log and continue, but could throw or stop.
                }
            }
        }

        // --- End Recording ---
        VK_CHECK(vkEndCommandBuffer(commandBuffer),
                 "Failed to record command buffer!");
        // LOG_TRACE("Command buffer recording finished.");
    }

} // namespace Bcg