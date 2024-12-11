//
// Created by alex on 12/12/24.
//

#ifndef IRENDERPASS_H
#define IRENDERPASS_H

namespace Bcg::Graphics {
    struct AttachmentDescription {
        Format format;
        LoadOp loadOp;
        StoreOp storeOp;
        // Other properties
    };

    struct SubpassDescription {
        std::vector<uint32_t> colorAttachmentIndices;
        int32_t depthStencilAttachmentIndex;
        // Other properties
    };

    struct RenderPassDesc {
        std::vector<AttachmentDescription> attachments;
        std::vector<SubpassDescription> subpasses;
    };

    class IRenderPass {
    public:
        virtual ~IRenderPass() = default;
    };
}

#endif //IRENDERPASS_H
