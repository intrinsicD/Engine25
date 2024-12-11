//
// Created by alex on 12/12/24.
//

#ifndef ICOMMANDBUFFER_H
#define ICOMMANDBUFFER_H

namespace Bcg::Graphics {
    class ICommandBuffer {
    public:
        virtual ~ICommandBuffer() = default;

        virtual void begin() = 0;
        virtual void end() = 0;

        // Recording commands
        virtual void bindPipeline(IPipeline* pipeline) = 0;
        virtual void bindVertexBuffer(IBuffer* buffer, uint32_t binding, uint64_t offset) = 0;
        virtual void bindIndexBuffer(IBuffer* buffer, IndexType indexType, uint64_t offset) = 0;
        virtual void bindDescriptorSet(IDescriptorSet* descriptorSet, uint32_t setIndex) = 0;

        virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
        virtual void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;

        // Resource barriers and synchronization
        virtual void resourceBarrier(const ResourceBarrierDesc& barrierDesc) = 0;

        // Command submission
        virtual void submit(IFence* fence = nullptr) = 0;
    };
}

#endif //ICOMMANDBUFFER_H
