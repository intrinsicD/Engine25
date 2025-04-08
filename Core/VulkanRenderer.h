//
// Created by alex on 08.04.25.
//

#ifndef ENGINE25_VULKANRENDERER_H
#define ENGINE25_VULKANRENDERER_H

#include "IModule.h"
#include <vector>
#include <memory>
#include <cstdint>
#include <optional> // Keep optional for QueueFamilyIndices
#include <vulkan/vulkan.h> // Include Vulkan headers

// Forward declarations
struct GLFWwindow; // Keep if chooseSwapExtent needs it directly
namespace Bcg {

    struct ApplicationContext; // Forward declare
    struct VulkanContext;      // Forward declare internal context struct

    // --- RenderPass Interface ---
    class RenderPass { // No longer an IModule itself, just an interface
    public:
        explicit RenderPass(std::string name) : m_passName(std::move(name)) {}

        virtual ~RenderPass() = default;

        const std::string &getName() const { return m_passName; }

        // Initialization specific to the pass (e.g., create pipelines)
        virtual bool Initialize(VulkanContext &vk_ctx, ApplicationContext *app_context) = 0;

        // Cleanup specific to the pass
        virtual void Shutdown(VulkanContext &vk_ctx) = 0;

        // Record draw commands into the provided command buffer
        virtual void Execute(VulkanContext &vk_ctx, VkCommandBuffer commandBuffer, uint32_t imageIndex) = 0;

        // Handle swapchain recreation - recreate pass-specific resources (framebuffers, pipelines)
        virtual void OnSwapChainCreate(VulkanContext &vk_ctx) = 0;

        // Handle swapchain cleanup - destroy pass-specific resources before swapchain views are gone
        virtual void OnSwapChainDestroy(VulkanContext &vk_ctx) = 0;

    protected:
        std::string m_passName;
    };

    // --- VulkanRenderer Class ---
    // Inherits from IModule for Application integration
    class VulkanRenderer : public IModule {
    public:
        VulkanRenderer();

        ~VulkanRenderer() override; // Ensure virtual destructor

        // --- IModule Overrides ---
        bool initialize(ApplicationContext *context) override;

        void shutdown() override;

        void update(float deltaTime) override; // Keep for potential internal updates
        void render() override;                // Main render loop trigger
        void renderUI() override;              // Specific UI rendering step (e.g., ImGui)
        // connect/disconnectDispatcher might be used if renderer listens to events
        // void connectDispatcher() override;
        // void disconnectDispatcher() override;

        // --- Public API ---
        void AddRenderPass(std::shared_ptr<RenderPass> pass);
        // Potentially: RemoveRenderPass, GetRenderPass, etc.

        // Public access for platform resize events
        void FramebufferResizeCallback();

        // --- Accessor for internal context (Use with caution) ---
        // Primarily for systems that *absolutely* need direct access, like maybe UI
        VulkanContext *GetVulkanContext(); // Non-const access

    private: // Internal implementation details
        friend struct VulkanContext; // Allow VulkanContext to access private members
        // --- Helper Structs (Internal) ---
        struct QueueFamilyIndices {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;

            bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
        };

        struct SwapChainSupportDetails {
            VkSurfaceCapabilitiesKHR capabilities{};
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        // --- Core Vulkan Setup (Private) ---
        bool createInstance();

        void setupDebugMessenger();

        bool createSurface();

        bool pickPhysicalDevice();

        bool createLogicalDevice();

        bool createCommandPool();

        bool createSyncObjects();

        bool allocateCommandBuffers(); // Allocate the main command buffer(s)

        // --- Swapchain Management (Private) ---
        void CreateSwapChain();

        void CreateSwapchainImageViews();

        void CleanupSwapChain(); // Cleans views, swapchain obj, notifies passes
        void RecreateSwapChain(); // Orchestrates cleanup and creation

        // --- Resource Cleanup (Private) ---
        void cleanupSyncObjects();

        void cleanupCommands();

        void cleanupCore(); // Instance, device, surface etc.


        // --- Internal Helper Functions (Private Static or Member) ---
        static std::vector<const char *> getRequiredExtensions();

        static bool checkValidationLayerSupport(const std::vector<const char *> &layers);

        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

        static VkResult
        CreateDebugUtilsMessengerEXT(VkInstance inst, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                     const VkAllocationCallbacks *pAllocator,
                                     VkDebugUtilsMessengerEXT *pDebugMessenger);

        static void DestroyDebugUtilsMessengerEXT(VkInstance inst, VkDebugUtilsMessengerEXT debugMessenger,
                                                  const VkAllocationCallbacks *pAllocator);

        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

        static bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

        bool isDeviceSuitable(VkPhysicalDevice device); // Now uses member surface

        static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D
        chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities); // Now uses member window ptr from context

        // --- Frame Rendering Logic (Private) ---
        void RecordCommandBuffer(uint32_t imageIndex); // Records commands from RenderPasses


        // --- Member Variables ---
        std::unique_ptr<VulkanContext> m_vkContext; // Owns the Vulkan state
        std::vector<std::shared_ptr<RenderPass>> m_render_passes;
        bool m_framebufferResized = false;

        // Max frames in flight (adjust as needed, 1 is simplest)
        const int MAX_FRAMES_IN_FLIGHT = 1;
        uint32_t m_currentFrame = 0;

        // Store validation layers for convenience
        const std::vector<const char *> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

// Enable validation layers in debug builds
#ifndef NDEBUG
        static const bool m_enableValidationLayers = true;
#else
        static const bool m_enableValidationLayers = false;
#endif

    }; // End VulkanRenderer Class

    struct VulkanContext {
        VkInstance instance{};
        VkDebugUtilsMessengerEXT debugMessenger{};
        VkSurfaceKHR surface{};
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device{}; // Logical device
        VulkanRenderer::QueueFamilyIndices queueFamilyIndices; // Store indices
        VkQueue graphicsQueue{};
        VkQueue presentQueue{};

        // Swapchain
        VkSwapchainKHR swapChain{};
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat{};
        VkExtent2D swapChainExtent{};
        std::vector<VkImageView> swapChainImageViews;

        // Command Pool & Buffers (Example: One per frame in flight)
        VkCommandPool commandPool{};
        std::vector<VkCommandBuffer> commandBuffers; // Size = MAX_FRAMES_IN_FLIGHT

        // Synchronization (Example: One set per frame in flight)
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        // Add other shared resources if needed (e.g., common descriptor pool)
    };

} // End namespace Bcg

#endif //ENGINE25_VULKANRENDERER_H