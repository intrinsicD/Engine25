//
// Created by alex on 03.04.25.
//

#ifndef ENGINE25_GRAPHICSAPI_H
#define ENGINE25_GRAPHICSAPI_H

#include <cstdint>
#include <vector>
#include <limits>
#include "entt/entity/fwd.hpp"

namespace Bcg{
    // Enum class for type safety and scoping
    enum class ClearFlag : uint8_t {
        None = 0,
        Color = 1 << 0,
        Depth = 1 << 1,
        Stencil = 1 << 2
    };

    // Enable bitwise operations for the enum class
    inline ClearFlag operator|(ClearFlag lhs, ClearFlag rhs) {
        return static_cast<ClearFlag>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
    }

    inline ClearFlag &operator|=(ClearFlag &lhs, ClearFlag rhs) {
        lhs = lhs | rhs;
        return lhs;
    }

    inline ClearFlag operator&(ClearFlag lhs, ClearFlag rhs) {
        return static_cast<ClearFlag>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
    }

    inline bool hasFlag(ClearFlag flags, ClearFlag flag) {
        return (static_cast<uint8_t>(flags) & static_cast<uint8_t>(flag)) != 0;
    }

    // --- Opaque Handles (Example implementation) ---
    struct BufferHandle {
        uint64_t id = 0;
    };
    struct TextureHandle {
        uint64_t id = 0;
    };
    struct SamplerHandle {
        uint64_t id = 0;
    };
    struct ShaderHandle {
        uint64_t id = 0;
    };
    struct PipelineLayoutHandle {
        uint64_t id = 0;
    }; // Resource binding layout
    struct PipelineHandle {
        uint64_t id = 0;
    };      // Graphics or Compute Pipeline State Object
    struct DescriptorSetLayoutHandle {
        uint64_t id = 0;
    };
    struct DescriptorSetHandle {
        uint64_t id = 0;
    };
    // Add more as needed (FramebufferHandle, RenderPassHandle for Vulkan, etc.)

    // --- Enums and Structs for Descriptions ---
    enum class BufferUsage {
        VERTEX_BUFFER, INDEX_BUFFER, UNIFORM_BUFFER, STORAGE_BUFFER, TRANSFER_SRC, TRANSFER_DST
    };
    enum class MemoryUsage {
        GPU_ONLY, CPU_TO_GPU, GPU_TO_CPU // Hints for memory allocation
    };
    enum class TextureUsage {
        SAMPLED, STORAGE, COLOR_ATTACHMENT, DEPTH_STENCIL_ATTACHMENT, TRANSFER_SRC, TRANSFER_DST
    };
    enum class TextureFormat {
        R8G8B8A8_UNORM, D32_SFLOAT, BC7_UNORM
    };
    enum class ShaderStage {
        VERTEX, FRAGMENT, COMPUTE, GEOMETRY, TESS_CONTROL, TESS_EVAL
    };
// ... (Many more enums for filtering, addressing, blending, comparison ops, etc.)

    struct BufferDesc {
        uint64_t size = 0;
        std::vector<BufferUsage> usage;
        MemoryUsage memoryUsage = MemoryUsage::GPU_ONLY;
    };

    struct TextureDesc {
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t depthOrArrayLayers = 1;
        uint32_t mipLevels = 1;
        TextureFormat format = TextureFormat::R8G8B8A8_UNORM; // Example default
        std::vector<TextureUsage> usage;
        // Add dimension (1D, 2D, 3D, Cube), samples (for MSAA)
    };

    // Simplified - Real pipeline state is complex!
    struct GraphicsPipelineDesc {
        PipelineLayoutHandle layout;
        ShaderHandle vertexShader;
        ShaderHandle fragmentShader;
        // Add: RasterizationState, DepthStencilState, BlendState, VertexInputLayout, PrimitiveTopology, RenderPass info...
    };

    struct ComputePipelineDesc {
        PipelineLayoutHandle layout;
        ShaderHandle computeShader;
    };

    struct DescriptorSetLayoutBinding {
        uint32_t binding;
        // DescriptorType type; // e.g., UNIFORM_BUFFER, SAMPLED_TEXTURE, STORAGE_IMAGE
        uint32_t descriptorCount;
        ShaderStage stageFlags; // Which shader stages can access this
    };


    //------------------------------------------------------------------------------------------------------------------
    // ICommandList interface
    //----------------------------------------------------------------------------------------------------------------

    class ICommandList {
    public:
        virtual ~ICommandList() = default;

        // --- State Binding & Drawing ---
        // virtual void bindPipeline(PipelineBindPoint bindPoint, PipelineHandle pipeline) = 0;
        // virtual void bindVertexBuffer(uint32_t firstBinding, BufferHandle buffer, uint64_t offset) = 0;
        // virtual void bindIndexBuffer(BufferHandle buffer, uint64_t offset, IndexType indexType) = 0;
        // virtual void bindDescriptorSet(...) = 0;
        // virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
        // virtual void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;
        // ... other necessary drawing/state commands ...

        // --- Clear Command ---
        virtual void clear(ClearFlag flags, const float color[4], float depth, uint32_t stencil) = 0;

        // Optional: Reset state at frame start if needed by implementation
        virtual void resetForNewFrame() {};
    };

    //---------------------------------------------------------------------------------------------------------------
    // FrameContext struct
    //---------------------------------------------------------------------------------------------------------------

    struct FrameContext{
        ICommandList *pCommandList = nullptr;

        int frameIndex = 0;
    };

    //---------------------------------------------------------------------------------------------------------------
    // IDevice interface
    //---------------------------------------------------------------------------------------------------------------

    class IDevice{
    public:
        // --- Resource Creation ---
        virtual BufferHandle createBuffer(const BufferDesc &desc, const void *initialData = nullptr) = 0;

        virtual TextureHandle createTexture(const TextureDesc &desc, const void *initialData = nullptr) = 0;

        virtual SamplerHandle createSampler(/* SamplerDesc desc */) = 0;

        virtual ShaderHandle
        createShader(/* ShaderStage stage, const std::vector<uint8_t>& code or std::string path */) = 0;

        virtual PipelineLayoutHandle
        createPipelineLayout(/* std::vector<DescriptorSetLayoutHandle> setLayouts, PushConstantRanges... */) = 0;

        virtual PipelineHandle createGraphicsPipeline(const GraphicsPipelineDesc &desc) = 0;

        virtual PipelineHandle createComputePipeline(const ComputePipelineDesc &desc) = 0;

        virtual DescriptorSetLayoutHandle
        createDescriptorSetLayout(const std::vector<DescriptorSetLayoutBinding> &bindings) = 0;

        virtual DescriptorSetHandle
        allocateDescriptorSet(DescriptorSetLayoutHandle layout) = 0; // Needs a pool internally or passed in

        // --- Resource Updates ---
        virtual void updateBuffer(BufferHandle buffer, uint64_t offset, uint64_t size, const void *data) = 0;

        virtual void
        updateDescriptorSet(DescriptorSetHandle set, uint32_t binding, uint32_t arrayElement, BufferHandle buffer,
                            uint64_t offset = 0, uint64_t range = std::numeric_limits<uint64_t>::max()) = 0; // Example overload
        virtual void
        updateDescriptorSet(DescriptorSetHandle set, uint32_t binding, uint32_t arrayElement, TextureHandle texture,
                            SamplerHandle sampler/*, TextureLayout layout*/) = 0; // Example overload

        // --- Resource Destruction ---
        virtual void destroyBuffer(BufferHandle handle) = 0;

        virtual void destroyTexture(TextureHandle handle) = 0;

        virtual void destroySampler(SamplerHandle handle) = 0;

        virtual void destroyShader(ShaderHandle handle) = 0;

        virtual void destroyPipelineLayout(PipelineLayoutHandle handle) = 0;

        virtual void destroyPipeline(PipelineHandle handle) = 0;

        virtual void destroyDescriptorSetLayout(DescriptorSetLayoutHandle handle) = 0;
        // Note: Descriptor sets are often freed by destroying their pool in Vulkan

        // --- Mapping (for CPU_TO_GPU or GPU_TO_CPU memory) ---
        virtual void *mapBuffer(BufferHandle handle, uint64_t offset, uint64_t size) = 0;

        virtual void unmapBuffer(BufferHandle handle) = 0;

        // --- Utility ---
        // Wait until all previously submitted GPU work is finished. Use sparingly!
        virtual void waitIdle() = 0;
    };

    //---------------------------------------------------------------------------------------------------------------
    // IRenderPass interface
    //---------------------------------------------------------------------------------------------------------------

    class IRenderPass {
    public:
        virtual ~IRenderPass() = default;
        // Setup resources needed specifically for this pass (e.g., pipelines, maybe framebuffers)
        virtual void setup(IDevice* device) = 0;
        // Record commands for this pass into the command list for the current frame
        virtual void execute(FrameContext& frameContext, entt::registry& scene) = 0;
        // Add resize handling if the pass manages resolution-dependent resources like framebuffers
        // virtual void onResize(uint32_t width, uint32_t height) {}
    };
}

#endif //ENGINE25_GRAPHICSAPI_H
