//
// Created by alex on 08.04.25.
//

#include "MeshRenderPass.h"
#include "ApplicationContext.h" // For accessing scene, swapchain extent etc.
#include "Logger.h"             // For logging
#include "VulkanRenderer.h"     // For VulkanContext definition and VK_CHECK
#include "SlangShaderModule.h"     // For VulkanContext definition and VK_CHECK

#include <stdexcept>          // For runtime_error
#include <vector>
#include <array>

// Define VK_CHECK locally if not accessible globally (though it should be via VulkanRenderer.h -> VulkanRenderer.cpp)
#ifndef VK_CHECK
#define VK_CHECK(result, msg)                                         \
    if (result != VK_SUCCESS) {                                       \
        LOG_ERROR(fmt::format("{}: {}", msg, static_cast<int>(result))); \
        throw std::runtime_error(msg);                                \
    }
#endif

namespace Bcg {

    // --- Private Helper Declarations ---
    namespace { // Use anonymous namespace for internal linkage

        void CreateInternalRenderPass(VulkanContext &vk_ctx, VkRenderPass &renderPass);
        void CreateFramebuffers(VulkanContext &vk_ctx, VkRenderPass renderPass, std::vector<VkFramebuffer>& framebuffers);
        void CreatePipelineLayout(VulkanContext &vk_ctx, VkPipelineLayout &pipelineLayout);
        void CreateGraphicsPipeline(VulkanContext &vk_ctx, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkPipeline &pipeline, ApplicationContext *appContext);
        void CleanupPipelineAndLayout(VulkanContext &vk_ctx, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout);
        void CleanupFramebuffers(VulkanContext &vk_ctx, std::vector<VkFramebuffer>& framebuffers);
        void CleanupRenderPass(VulkanContext &vk_ctx, VkRenderPass& renderPass);

        // --- Shader Loading Placeholder ---
        // In a real implementation, load SPIR-V bytecode here
        VkShaderModule createPlaceholderShaderModule(VkDevice device) {
            // This is a HACK: Returns NULL, which will likely prevent pipeline creation.
            // Replace with actual shader loading.
            LOG_WARN("Using placeholder (NULL) shader module. Graphics pipeline creation will likely fail or be skipped.");
            return VK_NULL_HANDLE;
        }

    } // anonymous namespace


    // --- MeshRenderPass Implementation ---

    MeshRenderPass::MeshRenderPass() : RenderPass("MeshRenderPass") {}

    bool MeshRenderPass::Initialize(VulkanContext &vk_ctx, ApplicationContext *app_context) {
        LOG_INFO("Initializing MeshRenderPass...");
        m_appContext = app_context; // Store context

        if (!vk_ctx.device) {
            LOG_ERROR("MeshRenderPass::Initialize - Invalid Vulkan device context.");
            return false;
        }


        try {
            // Create resources NOT dependent on swapchain first
            // Note: Render Pass *does* depend on the swapchain *format*,
            // but we can create it here assuming the format won't change drastically.
            // If format *can* change on resize, render pass needs recreation too.
            CreateInternalRenderPass(vk_ctx, m_renderPass);
            CreatePipelineLayout(vk_ctx, m_pipelineLayout);

            // Pipeline creation depends on render pass AND swapchain extent (for viewport/scissor usually)
            // So, defer pipeline creation to OnSwapChainCreate.

        } catch (const std::runtime_error& e) {
            LOG_ERROR(fmt::format("MeshRenderPass Initialization failed: {}", e.what()));
            Shutdown(vk_ctx); // Clean up partially created resources
            return false;
        }

        LOG_INFO("MeshRenderPass Initialized (pipeline/framebuffers pending swapchain).");
        return true;
    }

    void MeshRenderPass::Shutdown(VulkanContext &vk_ctx) {
        LOG_INFO("Shutting down MeshRenderPass...");
        if (!vk_ctx.device) {
            LOG_WARN("MeshRenderPass::Shutdown - Vulkan device context already invalid.");
            // Clear handles manually if needed, though they should be VK_NULL_HANDLE
            m_graphicsPipeline = VK_NULL_HANDLE;
            m_pipelineLayout = VK_NULL_HANDLE;
            m_renderPass = VK_NULL_HANDLE;
            m_swapChainFramebuffers.clear();
            return;
        }

        // Ensure GPU is idle before destroying resources owned by this pass
        // Note: The main renderer shutdown should call vkDeviceWaitIdle,
        // but adding it here provides safety if pass is shut down independently.
        // vkDeviceWaitIdle(vk_ctx.device); // Optional: Depends on usage context

        // Destroy in reverse order of creation / dependency
        // Swapchain-dependent resources should be cleaned by OnSwapChainDestroy first
        CleanupFramebuffers(vk_ctx, m_swapChainFramebuffers); // Should be empty if OnSwapChainDestroy was called
        CleanupPipelineAndLayout(vk_ctx, m_graphicsPipeline, m_pipelineLayout); // Destroy pipeline and its layout
        CleanupRenderPass(vk_ctx, m_renderPass); // RenderPass is last

        m_appContext = nullptr; // Clear stored context
        LOG_INFO("MeshRenderPass Shutdown complete.");
    }

    void MeshRenderPass::OnSwapChainCreate(VulkanContext &vk_ctx) {
        LOG_INFO("MeshRenderPass handling SwapChain Creation...");
        if (!vk_ctx.device || m_renderPass == VK_NULL_HANDLE || m_pipelineLayout == VK_NULL_HANDLE) {
            LOG_ERROR("MeshRenderPass::OnSwapChainCreate - Dependencies not ready (device/renderpass/layout missing).");
            return;
        }

        try {
            // Recreate resources dependent on swapchain views/extent
            CreateFramebuffers(vk_ctx, m_renderPass, m_swapChainFramebuffers);
            CreateGraphicsPipeline(vk_ctx, m_renderPass, m_pipelineLayout, m_graphicsPipeline, m_appContext);
            LOG_INFO("MeshRenderPass swapchain resources recreated.");
        } catch (const std::runtime_error& e) {
            LOG_ERROR(fmt::format("MeshRenderPass OnSwapChainCreate failed: {}", e.what()));
            // Clean up potentially partially created resources
            CleanupFramebuffers(vk_ctx, m_swapChainFramebuffers);
            CleanupPipelineAndLayout(vk_ctx, m_graphicsPipeline, m_pipelineLayout);
            // Consider how to recover - maybe the pass becomes inactive?
        }
    }

    void MeshRenderPass::OnSwapChainDestroy(VulkanContext &vk_ctx) {
        LOG_INFO("MeshRenderPass handling SwapChain Destruction...");
        if (!vk_ctx.device) {
            LOG_WARN("MeshRenderPass::OnSwapChainDestroy - Vulkan device context already invalid.");
            // Clear handles manually if needed
            m_graphicsPipeline = VK_NULL_HANDLE; // Pipeline definitely invalid now
            m_swapChainFramebuffers.clear();     // Framebuffers definitely invalid now
            return;
        }

        // Destroy resources dependent on the swapchain *before* the views are gone
        // Wait idle might be needed here if resources are actively used. Renderer usually handles this.
        // vkDeviceWaitIdle(vk_ctx.device);

        CleanupFramebuffers(vk_ctx, m_swapChainFramebuffers);
        // Destroy the pipeline as it depends on extent/renderpass format which might change
        if (m_graphicsPipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(vk_ctx.device, m_graphicsPipeline, nullptr);
            m_graphicsPipeline = VK_NULL_HANDLE;
        }
        // Keep RenderPass and PipelineLayout, they might be reusable if format/layout doesn't change.
        // They will be cleaned up in Shutdown().
        LOG_INFO("MeshRenderPass swapchain-dependent resources destroyed.");
    }

    void MeshRenderPass::Execute(VulkanContext &vk_ctx, VkCommandBuffer commandBuffer, uint32_t imageIndex) {
        // --- Check Prerequisites ---
        if (m_renderPass == VK_NULL_HANDLE) {
            // LOG_TRACE("MeshRenderPass Execute skipped: RenderPass handle is null.");
            return;
        }
        // Need a valid pipeline to draw anything (even clear needs it implicitly via render pass)
        if (m_graphicsPipeline == VK_NULL_HANDLE) {
            LOG_TRACE("MeshRenderPass Execute skipped: GraphicsPipeline handle is null.");
            // We might still want to execute the render pass just for the clear operation
            // defined in the loadOp, even without a pipeline. Let's allow that for now.
            // If drawing is required, add the check here: return;
        }
        if (m_swapChainFramebuffers.empty()) {
            // LOG_TRACE("MeshRenderPass Execute skipped: Framebuffers not created.");
            return;
        }
        if (imageIndex >= m_swapChainFramebuffers.size()) {
            LOG_ERROR(fmt::format("MeshRenderPass Execute error: Invalid imageIndex {} for {} framebuffers.",
                                  imageIndex, m_swapChainFramebuffers.size()));
            return; // Invalid index, cannot proceed
        }
        if (m_swapChainFramebuffers[imageIndex] == VK_NULL_HANDLE) {
            LOG_ERROR(fmt::format("MeshRenderPass Execute error: Framebuffer at index {} is VK_NULL_HANDLE.", imageIndex));
            return; // Framebuffer for this image is invalid
        }

        // --- Begin Render Pass ---
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_swapChainFramebuffers[imageIndex]; // Target the correct framebuffer
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = vk_ctx.swapChainExtent;

        // Define clear values (matches the attachments in CreateInternalRenderPass)
        std::array<VkClearValue, 1> clearValues; // Increase size if depth attachment added
        clearValues[0].color = {{0.15f, 0.15f, 0.18f, 1.0f}}; // Slightly bluish dark grey
        // clearValues[1].depthStencil = {1.0f, 0}; // If depth attachment exists (index 1)

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // --- Set Dynamic State ---
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f; // Can be flipped for inverted Y if needed: (float)vk_ctx.swapChainExtent.height;
        viewport.width = static_cast<float>(vk_ctx.swapChainExtent.width);
        viewport.height = static_cast<float>(vk_ctx.swapChainExtent.height); // Can be flipped: -(float)vk_ctx.swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = vk_ctx.swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        // --- Bind Pipeline (if valid) ---
        if (m_graphicsPipeline != VK_NULL_HANDLE) {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

            // --- Bind Resources & Draw (Placeholders) ---
            // vkCmdBindDescriptorSets(...);
            // vkCmdBindVertexBuffers(...);
            // vkCmdBindIndexBuffer(...);
            // vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);

            // --- Simple Draw Command (draws nothing without shaders/vertex input) ---
            vkCmdDraw(commandBuffer, 3, 1, 0, 0); // Draw 3 vertices, 1 instance
        } else {
            // Logged warning during execute if pipeline is null but we proceeded for clear.
            // LOG_TRACE("MeshRenderPass: Pipeline is null, skipping bind and draw commands.");
        }

        // --- End Render Pass ---
        vkCmdEndRenderPass(commandBuffer);
    }


    // --- Private Helper Implementations ---
    namespace { // Use anonymous namespace

        void CreateInternalRenderPass(VulkanContext &vk_ctx, VkRenderPass &renderPass) {
            if (renderPass != VK_NULL_HANDLE) {
                LOG_WARN("MeshRenderPass::CreateInternalRenderPass - RenderPass handle already exists, destroying old one.");
                vkDestroyRenderPass(vk_ctx.device, renderPass, nullptr);
                renderPass = VK_NULL_HANDLE;
            }
            LOG_INFO("Creating MeshRenderPass Internal RenderPass Object...");

            // Attachment: Color (Matches swapchain)
            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = vk_ctx.swapChainImageFormat;       // Use the format Vulkan gave us
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;          // No multisampling
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;       // Clear when render pass begins
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;      // Store results for presentation
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // No stencil
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;      // Don't care about previous layout
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // ** Crucial for presentation **

            // TODO: Add Depth Attachment if needed
            // VkAttachmentDescription depthAttachment{};
            // depthAttachment.format = findDepthFormat(vk_ctx.physicalDevice); // Helper needed
            // depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            // depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            // depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Often don't need to store depth
            // depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            // depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            // depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            // depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            // References to attachments for the subpass
            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = 0; // Index in the pAttachments array (above)
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Layout during subpass

            // VkAttachmentReference depthAttachmentRef{};
            // depthAttachmentRef.attachment = 1; // Index 1 if depth is added
            // depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            // Subpass Description
            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
            // subpass.pDepthStencilAttachment = &depthAttachmentRef; // Assign if depth used

            // Subpass Dependency: Ensures layout transitions happen correctly
            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // Implicit subpass before this render pass
            dependency.dstSubpass = 0;                 // Our first (and only) subpass
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Wait stage in src (before pass)
            // dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; // If depth used
            dependency.srcAccessMask = 0;              // No access needed before pass starts (layout is UNDEFINED)
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Wait stage in dst (our subpass)
            // dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; // If depth used
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;       // Access needed in our subpass
            // dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; // If depth used

            std::array<VkAttachmentDescription, 1> attachments = {colorAttachment}; // Add depthAttachment if used
            // std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

            // Render Pass Create Info
            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            renderPassInfo.pAttachments = attachments.data();
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;

            VK_CHECK(vkCreateRenderPass(vk_ctx.device, &renderPassInfo, nullptr, &renderPass),
                     "Failed to create MeshRenderPass render pass!");
            LOG_INFO("MeshRenderPass internal RenderPass object created.");
        }

        void CreateFramebuffers(VulkanContext &vk_ctx, VkRenderPass renderPass, std::vector<VkFramebuffer>& framebuffers) {
            LOG_INFO("Creating MeshRenderPass Framebuffers...");
            CleanupFramebuffers(vk_ctx, framebuffers); // Clean old ones first

            if (renderPass == VK_NULL_HANDLE) {
                throw std::runtime_error("Cannot create framebuffers: RenderPass handle is null.");
            }
            if (vk_ctx.swapChainImageViews.empty()) {
                throw std::runtime_error("Cannot create framebuffers: Swapchain image views are empty.");
            }


            framebuffers.resize(vk_ctx.swapChainImageViews.size());

            for (size_t i = 0; i < vk_ctx.swapChainImageViews.size(); i++) {
                if (vk_ctx.swapChainImageViews[i] == VK_NULL_HANDLE) {
                    LOG_ERROR(fmt::format("Cannot create framebuffer {}: ImageView handle is null.", i));
                    // Set corresponding framebuffer handle to null to indicate failure for this specific one
                    framebuffers[i] = VK_NULL_HANDLE;
                    continue; // Skip creation for this invalid view
                }

                std::array<VkImageView, 1> attachments = { // Add depth view if used
                        vk_ctx.swapChainImageViews[i]
                        // , depthImageView // Assuming depthImageView is created elsewhere
                };

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = renderPass; // Use the pass this framebuffer is compatible with
                framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
                framebufferInfo.pAttachments = attachments.data();
                framebufferInfo.width = vk_ctx.swapChainExtent.width;
                framebufferInfo.height = vk_ctx.swapChainExtent.height;
                framebufferInfo.layers = 1; // For non-stereoscopic rendering

                VK_CHECK(vkCreateFramebuffer(vk_ctx.device, &framebufferInfo, nullptr, &framebuffers[i]),
                         fmt::format("Failed to create MeshRenderPass framebuffer {}!", i));
            }
            LOG_INFO(fmt::format("Created {} MeshRenderPass framebuffers (potential null handles if image views were invalid).", framebuffers.size()));
        }

        void CreatePipelineLayout(VulkanContext &vk_ctx, VkPipelineLayout &pipelineLayout) {
            if (pipelineLayout != VK_NULL_HANDLE) {
                LOG_WARN("MeshRenderPass::CreatePipelineLayout - Layout handle already exists, destroying old one.");
                vkDestroyPipelineLayout(vk_ctx.device, pipelineLayout, nullptr);
                pipelineLayout = VK_NULL_HANDLE;
            }
            LOG_INFO("Creating MeshRenderPass Pipeline Layout...");

            // Define based on shader needs (descriptor set layouts, push constants)
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0; // No descriptor sets for now
            pipelineLayoutInfo.pSetLayouts = nullptr;
            pipelineLayoutInfo.pushConstantRangeCount = 0; // No push constants for now
            pipelineLayoutInfo.pPushConstantRanges = nullptr;

            VK_CHECK(vkCreatePipelineLayout(vk_ctx.device, &pipelineLayoutInfo, nullptr, &pipelineLayout),
                     "Failed to create MeshRenderPass pipeline layout!");
            LOG_INFO("MeshRenderPass Pipeline Layout created (empty).");
        }

        void CreateGraphicsPipeline(VulkanContext &vk_ctx, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkPipeline &pipeline, ApplicationContext* appContext) {
            // ... (pipeline cleanup, check dependencies) ...
            if (!appContext || !appContext->shaderModule) {
                LOG_ERROR("MeshRenderPass::CreateGraphicsPipeline - ApplicationContext or SlangShaderModule is null.");
                pipeline = VK_NULL_HANDLE; // Ensure pipeline handle is null
                return;
            }
            LOG_INFO("Creating MeshRenderPass Graphics Pipeline using Slang...");


            // --- Use SlangShaderModule to get VkShaderModules ---
            // Replace "shaders/simple.slang" and entry points with your actual file/names
            VkShaderModule vertShaderModule = appContext->shaderModule->CreateVkShaderModuleFromFile(
                    vk_ctx,
                    std::string(SOURCE_DIR_PATH) + "/shaders/simple.slang", // Example path construction
                    "vertexMain",
                    VK_SHADER_STAGE_VERTEX_BIT
                    // Optional: Add defines vector here
                    // Optional: Set forceRecompile = true for debugging
            );

            VkShaderModule fragShaderModule = appContext->shaderModule->CreateVkShaderModuleFromFile(
                    vk_ctx,
                    std::string(SOURCE_DIR_PATH) + "/shaders/simple.slang", // Example path construction
                    "fragmentMain",
                    VK_SHADER_STAGE_FRAGMENT_BIT
            );

            // *** Check if modules were created successfully ***
            if (vertShaderModule == VK_NULL_HANDLE || fragShaderModule == VK_NULL_HANDLE) {
                LOG_ERROR("MeshRenderPass::CreateGraphicsPipeline - Failed to create shader modules using Slang. Skipping pipeline creation.");
                // Cleanup any potentially created modules
                if (vertShaderModule) vkDestroyShaderModule(vk_ctx.device, vertShaderModule, nullptr);
                if (fragShaderModule) vkDestroyShaderModule(vk_ctx.device, fragShaderModule, nullptr);
                pipeline = VK_NULL_HANDLE; // Ensure pipeline handle is null
                return;
            }
            LOG_INFO("Slang shader modules created.");


            // --- Shader Stage Info (using the created modules) ---
            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            // ... (set sType, stage, pName as before) ...
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertShaderModule;
            vertShaderStageInfo.pName = "vertexMain";

            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            // ... (set sType, stage, pName as before) ...
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragShaderModule;
            fragShaderStageInfo.pName = "fragmentMain";

            VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

            // --- Fixed Functions ---
            // ... (VertexInput, InputAssembly, Viewport, Rasterizer, etc. as before) ...
            // *** IMPORTANT: Define VkPipelineVertexInputStateCreateInfo based on your shader's input needs ***
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{}; // Example: No input
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            vertexInputInfo.pVertexBindingDescriptions = nullptr;
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pVertexAttributeDescriptions = nullptr;
            // ... rest of fixed functions ...


            // --- Graphics Pipeline Create Info ---
            VkGraphicsPipelineCreateInfo pipelineInfo{};
            // ... (set members as before, using shaderStages) ...
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo; // Use your defined vertex input
            // ... set other states (pInputAssemblyState, pViewportState, etc.) ...

            VK_CHECK(vkCreateGraphicsPipelines(vk_ctx.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline),
                     "Failed to create MeshRenderPass graphics pipeline!");
            LOG_INFO("MeshRenderPass Graphics Pipeline created using Slang shaders.");

            // --- Cleanup Shader Modules ---
            // Crucial: Destroy the VkShaderModule handles now that they are linked into the pipeline
            if (vertShaderModule) vkDestroyShaderModule(vk_ctx.device, vertShaderModule, nullptr);
            if (fragShaderModule) vkDestroyShaderModule(vk_ctx.device, fragShaderModule, nullptr);
            LOG_INFO("Temporary VkShaderModules destroyed.");
        }

        void CleanupPipelineAndLayout(VulkanContext &vk_ctx, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout) {
            if (pipeline != VK_NULL_HANDLE) {
                vkDestroyPipeline(vk_ctx.device, pipeline, nullptr);
                pipeline = VK_NULL_HANDLE;
                LOG_TRACE("MeshRenderPass Graphics Pipeline destroyed.");
            }
            if (pipelineLayout != VK_NULL_HANDLE) {
                vkDestroyPipelineLayout(vk_ctx.device, pipelineLayout, nullptr);
                pipelineLayout = VK_NULL_HANDLE;
                LOG_TRACE("MeshRenderPass Pipeline Layout destroyed.");
            }
        }

        void CleanupFramebuffers(VulkanContext &vk_ctx, std::vector<VkFramebuffer>& framebuffers) {
            if (!framebuffers.empty()) {
                LOG_TRACE(fmt::format("Destroying {} MeshRenderPass Framebuffers...", framebuffers.size()));
                for (VkFramebuffer framebuffer : framebuffers) {
                    // Check handle is valid before destroying
                    if (framebuffer != VK_NULL_HANDLE) {
                        vkDestroyFramebuffer(vk_ctx.device, framebuffer, nullptr);
                    }
                }
                framebuffers.clear();
            }
        }

        void CleanupRenderPass(VulkanContext &vk_ctx, VkRenderPass& renderPass) {
            if (renderPass != VK_NULL_HANDLE) {
                vkDestroyRenderPass(vk_ctx.device, renderPass, nullptr);
                renderPass = VK_NULL_HANDLE;
                LOG_TRACE("MeshRenderPass RenderPass object destroyed.");
            }
        }

    } // anonymous namespace

} // namespace Bcg