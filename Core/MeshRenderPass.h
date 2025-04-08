//
// Created by alex on 08.04.25.
//

#ifndef ENGINE25_MESHRENDERPASS_H
#define ENGINE25_MESHRENDERPASS_H

#include "VulkanRenderer.h"

namespace Bcg{
    class MeshRenderPass : public RenderPass {
    public:
        MeshRenderPass();

        ~MeshRenderPass() override = default;

        bool Initialize(VulkanContext &vk_ctx, ApplicationContext *app_context) override;

        // Cleanup specific to the pass
        void Shutdown(VulkanContext &vk_ctx) override;

        // Record draw commands into the provided command buffer
        void Execute(VulkanContext &vk_ctx, VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

        // Handle swapchain recreation - recreate pass-specific resources (framebuffers, pipelines)
        void OnSwapChainCreate(VulkanContext &vk_ctx) override;

        // Handle swapchain cleanup - destroy pass-specific resources before swapchain views are gone
        void OnSwapChainDestroy(VulkanContext &vk_ctx) override;
    private:
// --- Member Variables ---
        ApplicationContext *m_appContext = nullptr; // Store pointer to global context if needed

        // Vulkan objects owned by this pass
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

        // *** ADDED MISSING MEMBERS ***
        VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;             // Handle for the graphics pipeline
        std::vector<VkFramebuffer> m_swapChainFramebuffers; // Framebuffers (one per swapchain image)
        // *** END ADDED MEMBERS ***
    };
}

#endif //ENGINE25_MESHRENDERPASS_H
