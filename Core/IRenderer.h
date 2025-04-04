//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_IRENDERER_H
#define ENGINE25_IRENDERER_H

#include <unordered_map>
#include "IModule.h"
#include "Frameworks/Graphics/GraphicsApi2.h"

namespace Bcg {
    struct FrameData {
        // Synchronization
        Graphics::FenceHandle       inFlightFence           = {Graphics::InvalidId64};
        Graphics::SemaphoreHandle   imageAvailableSemaphore   = {Graphics::InvalidId64};
        Graphics::SemaphoreHandle   renderFinishedSemaphore = {Graphics::InvalidId64};

        // Command Buffer (assuming one primary buffer per frame)
        Graphics::CommandBufferHandle commandBuffer         = {Graphics::InvalidId32};

        // Optional: Command Pool per frame (if resetting pools is preferred)
        // Graphics::CommandPoolHandle commandPool           = {Graphics::InvalidId32};
    };

    class IRenderer : public IModule {
    public:
        IRenderer(const char *name, const char *version) : IModule(name, version) {}

        ~IRenderer() override = default;

        bool initialize(ApplicationContext *context) override;

        void shutdown() override;

        void render() override;

        FrameData &getCurrentFrameData() const;

    protected:
        virtual void recreateSwapchain() = 0;

        bool m_needsResize = false;

        int MAX_FRAMES_IN_FLIGHT = 2;
        int m_currentFrame = 0;

        FrameData m_frameData[2];
        Graphics::Device *m_device;
        Graphics::SwapchainHandle m_swapchain;
        Graphics::FramebufferHandle m_framebuffers[];
    };
}

#endif //ENGINE25_IRENDERER_H
