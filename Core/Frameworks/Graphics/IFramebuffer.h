//
// Created by alex on 12/12/24.
//

#ifndef IFRAMEBUFFER_H
#define IFRAMEBUFFER_H

namespace Bcg::Graphics {
    struct RenderTargetDesc {
        ITexture *texture;
        uint32_t mipLevel;
        uint32_t arrayLayer;
    };

    struct FramebufferDesc {
        std::vector<RenderTargetDesc> colorAttachments;
        RenderTargetDesc depthStencilAttachment;
        IRenderPass *renderPass; // For Vulkan
    };

    class IFramebuffer {
    public:
        virtual ~IFramebuffer() = default;
    };
}
#endif //IFRAMEBUFFER_H
