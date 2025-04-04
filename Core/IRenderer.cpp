//
// Created by alex on 04.04.25.
//

#include "IRenderer.h"
#include "Frameworks/Graphics/GraphicsApi2.h"

namespace Bcg{
    using namespace Graphics;

    void IRenderer::render() {
        // --- 1. Prepare Frame ---
        FrameData& frameData = getCurrentFrameData(); // Get sync objects, command buffer for this frame index

        // Wait for previous use of this frame's resources
        m_device->wait_for_fences({frameData.inFlightFence}, true, UINT64_MAX);

        // Acquire image (Handles sync internally via semaphore)
        Uint32 imageIndex;
        bool swapchainValid = m_device->acquire_next_image(
                m_swapchain, // Swapchain handle owned by renderer
                UINT64_MAX,
                frameData.imageAvailableSemaphore, // Semaphore to signal
                {}, // No fence needed here usually
                imageIndex
        );

        if (!swapchainValid || m_needsResize) {
            recreateSwapchain(); // Handle resize or out-of-date
            return; // Skip rendering this frame
        }

        // Reset sync objects and command buffer for reuse
        m_device->reset_fences({frameData.inFlightFence});
        m_device->reset_command_buffer(frameData.commandBuffer);


        // --- 2. Record Commands ---
        CommandBufferHandle cmdbuf = frameData.commandBuffer;
        m_device->begin_command_buffer(cmdbuf);

        // ---> Application logic decides which passes run <---
        // Example:
        executeGeometryPass(cmdbuf, m_framebuffers[imageIndex], m_app_context->scene);
        executeLightingPass(cmdbuf, ...);
        executeUIPass(cmdbuf, m_framebuffers[imageIndex], m_uiManager->getDrawData());
        // OR use RenderPass objects if preferred:
        // for (auto& pass : m_renderPasses) {
        //     pass->execute(cmdbuf, m_framebuffers[imageIndex], scene/*, other context*/);
        // }

        m_device->end_command_buffer(cmdbuf);

        // --- 3. Submit ---
        SubmitInfo submitInfo = {};
        submitInfo.wait_semaphores = { frameData.imageAvailableSemaphore };
        PipelineStageFlags waitStages[] = { PipelineStageFlags::ColorAttachmentOutput };
        submitInfo.wait_dst_stage_masks = { waitStages[0] };
        submitInfo.command_buffers = { cmdbuf };
        submitInfo.signal_semaphores = { frameData.renderFinishedSemaphore };
        submitInfo.signal_fence = frameData.inFlightFence; // Signal fence when done

        m_device->queue_submit(/*queue,*/ {submitInfo}); // Submit to queue

        // --- 4. Present ---
        PresentInfo presentInfo = {};
        presentInfo.wait_semaphores = { frameData.renderFinishedSemaphore };
        presentInfo.swapchains = { m_swapchain };
        presentInfo.image_indices = { imageIndex };

        m_device->queue_present(/*queue,*/ presentInfo); // Present the image

        // --- 5. Advance Frame Index ---
        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}