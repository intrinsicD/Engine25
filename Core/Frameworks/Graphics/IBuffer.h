//
// Created by alex on 12/12/24.
//

#ifndef IBUFFER_H
#define IBUFFER_H

namespace Bcg::Graphics {
    enum class BufferUsage {
        VertexBuffer,
        IndexBuffer,
        UniformBuffer,
        StorageBuffer,
        // Add more as needed
    };

    struct BufferDesc {
        size_t size;
        BufferUsage usage;
        MemoryUsage memoryUsage; // GPU only, CPU only, CPU to GPU, etc.
    };

    class IBuffer {
    public:
        virtual ~IBuffer() = default;

        virtual void* map() = 0; // For CPU access
        virtual void unmap() = 0;
    };
}
#endif //IBUFFER_H
