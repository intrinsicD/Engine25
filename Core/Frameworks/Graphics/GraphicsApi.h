//
// Created by alex on 12.12.24.
//

#ifndef ENGINE25_GRAPHICSAPI_H
#define ENGINE25_GRAPHICSAPI_H

#include <vector>
#include <string>
#include <limits>
#include <cstddef>
#include <memory>

namespace Bcg::Graphics {

    // Aliases for basic types
    using Int32 = int;
    using Uint32 = unsigned int;
    using Uint64 = unsigned long long;
    using IndexType = unsigned int;

    // Enums
    enum class Format {
        R8,
        R16,
        R32,
        RG8,
        RG16,
        RG32,
        RGB8,
        RGB16,
        RGB32,
        RGBA8,
        RGBA16,
        RGBA32,
        Depth32F,
    };

    enum class PresentMode {
        Immediate,
        Mailbox,
        FIFO,
    };

    enum class LoadOp {
        Load,
        Clear,
        DontCare,
    };

    enum class StoreOp {
        Store,
        DontCare,
    };

    enum class PolygonMode {
        Fill,
        Line,
        Point,
    };

    enum class CullMode {
        None,
        Front,
        Back,
        FrontAndBack,
    };

    enum class BlendFactor {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstColor,
        OneMinusDstColor,
        DstAlpha,
        OneMinusDstAlpha,
    };

    enum class CompareOp {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always,
    };

    enum class Filter {
        Nearest,
        Linear,
    };

    enum class MipmapMode {
        Nearest,
        Linear,
    };

    enum class AddressMode {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
    };

    enum class BorderColor {
        TransparentBlack,
        OpaqueBlack,
        OpaqueWhite,
    };

    enum class TextureUsage {
        Sampled,
        Storage,
        RenderTarget,
        DepthStencil,
    };

    enum class StencilOp {
        Keep,
        Zero,
        Replace,
        IncrementClamp,
        DecrementClamp,
        Invert,
        IncrementWrap,
        DecrementWrap,
    };

    enum class FrontFace {
        Clockwise,
        CounterClockwise,
    };

    enum class BlendOp {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max,
    };

    enum class PipelineStage {
        TopOfPipe,
        DrawIndirect,
        VertexInput,
        VertexShader,
        FragmentShader,
        ComputeShader,
        Transfer,
        BottomOfPipe,
        Host,
    };

    enum class DescriptorType {
        UniformBuffer,
        StorageBuffer,
        SampledImage,
        CombinedImageSampler,
        StorageImage,
    };

    enum class AccessFlags {
        None = 0,
        Read = 1 << 0,               // Resource is read-only
        Write = 1 << 1,              // Resource is writable
        TransferRead = 1 << 2,       // Used in transfer (copy) operations for reading
        TransferWrite = 1 << 3,      // Used in transfer (copy) operations for writing
        ColorAttachmentRead = 1 << 4,    // Read from color attachment
        ColorAttachmentWrite = 1 << 5,   // Write to color attachment
        DepthStencilRead = 1 << 6,       // Read from depth/stencil attachment
        DepthStencilWrite = 1 << 7,      // Write to depth/stencil attachment
        ShaderRead = 1 << 8,         // Read in a shader stage
        ShaderWrite = 1 << 9,        // Write in a shader stage
        HostRead = 1 << 10,          // Host read access (CPU-visible)
        HostWrite = 1 << 11,         // Host write access (CPU-visible)
    };

    enum class DeviceFeature {
        MultiDrawIndirect,       // Supports multiple draw calls in a single command (e.g., glMultiDrawElementsIndirect)
        ComputeShaders,          // Supports compute shaders
        GeometryShaders,         // Supports geometry shaders
        TessellationShaders,     // Supports tessellation control and evaluation shaders
        RayTracing,              // Supports hardware-accelerated ray tracing
        VariableRateShading,     // Supports variable rate shading
        MeshShaders,             // Supports mesh and task shaders
        SparseResources,         // Supports sparse memory (tiled resources)
        DepthBoundsTest,         // Supports depth bounds testing
        TextureCompressionBC,    // Supports BC (Block Compression) texture formats
        TextureCompressionETC2,  // Supports ETC2 compressed texture formats
        TextureCompressionASTC,  // Supports ASTC compressed texture formats
        AnisotropicFiltering,    // Supports anisotropic texture filtering
        SampleRateShading,       // Supports shading at sample frequency in MSAA
        TimelineSemaphores,      // Supports timeline semaphores for advanced synchronization
        ConservativeRasterization, // Supports conservative rasterization
        WideLines,               // Supports wide lines for line rendering
        ShaderFloat16,           // Supports 16-bit floating-point arithmetic in shaders
        ShaderInt8,              // Supports 8-bit integer arithmetic in shaders
        ShaderInt64,             // Supports 64-bit integer arithmetic in shaders
        FragmentShaderInterlock, // Supports fragment shader interlock extensions
        BindlessTextures,        // Supports bindless texture arrays
        NonUniformResourceIndexing, // Supports non-uniform indexing in shaders
    };

    enum class ShaderStage {
        Vertex,
        Fragment,
        Compute,
        Geometry,
        TessellationControl,
        TessellationEvaluation,
    };

    enum class SampleCount {
        Count1 = 1,
        Count2 = 2,
        Count4 = 4,
        Count8 = 8,
        Count16 = 16,
    };

    enum class MemoryUsage {
        GpuOnly,
        CpuOnly,
        CpuToGpu,
        GpuToCpu
    };

    enum class BufferUsage {
        VertexBuffer,
        IndexBuffer,
        UniformBuffer,
        StorageBuffer,
    };

    enum class DeviceErrorType {
        None,
        OutOfMemory,
        InvalidOperation,
        ResourceCreationFailed,
        DeviceLost,
        Unknown,
    };

    enum class ResourceType {
        Buffer,
        Texture,
        Sampler,
        Unknown,
    };

    // Resource Descriptions
    struct TextureDesc {
        Uint32 width = 0;
        Uint32 height = 0;
        Uint32 depth = 1;
        Uint32 mipLevels = 1;
        Format format = Format::RGBA8;
        TextureUsage usage = TextureUsage::Sampled;
        Filter minFilter = Filter::Linear;
        Filter magFilter = Filter::Linear;
        AddressMode addressModeU = AddressMode::Repeat;
        AddressMode addressModeV = AddressMode::Repeat;
    };

    struct SamplerDesc {
        Filter minFilter = Filter::Linear;
        Filter magFilter = Filter::Linear;
        MipmapMode mipmapMode = MipmapMode::Linear;
        AddressMode addressModeU = AddressMode::Repeat;
        AddressMode addressModeV = AddressMode::Repeat;
        AddressMode addressModeW = AddressMode::Repeat;
        float mipLodBias = 0.0f;
        float maxAnisotropy = 1.0f;
        BorderColor borderColor = BorderColor::OpaqueBlack;
    };

    struct BufferDesc {
        size_t size = 0;
        MemoryUsage memoryUsage = MemoryUsage::GpuOnly;
        BufferUsage usage = BufferUsage::VertexBuffer;
    };

    class IResource;

    struct DescriptorBinding {
        Uint32 binding;
        DescriptorType type;
        Uint32 count;

        std::vector<std::shared_ptr<IResource>> resources;
    };

    struct RasterizationStateDesc {
        bool depthClampEnable;
        PolygonMode fillMode;
        CullMode cullMode;
        FrontFace frontFace;
        bool depthBiasEnable;
        float depthBiasConstantFactor;
        float depthBiasClamp;
        float depthBiasSlopeFactor;
    };

    struct DepthStencilStateDesc {
        bool depthTestEnable;
        bool depthWriteEnable;
        CompareOp depthCompareOp;
        bool stencilTestEnable;

        struct StencilOpDesc {
            StencilOp failOp;
            StencilOp passOp;
            StencilOp depthFailOp;
            CompareOp compareOp;
            Uint32 compareMask;
            Uint32 writeMask;
            Uint32 reference;
        };

        StencilOpDesc front;
        StencilOpDesc back;
    };

    struct BlendStateDesc {
        bool blendEnable;
        BlendFactor srcColorBlendFactor;
        BlendFactor dstColorBlendFactor;
        BlendOp colorBlendOp;
        BlendFactor srcAlphaBlendFactor;
        BlendFactor dstAlphaBlendFactor;
        BlendOp alphaBlendOp;

        struct RenderTargetBlendDesc {
            bool blendEnable;
            BlendFactor srcColorBlendFactor;
            BlendFactor dstColorBlendFactor;
            BlendOp colorBlendOp;
            BlendFactor srcAlphaBlendFactor;
            BlendFactor dstAlphaBlendFactor;
            BlendOp alphaBlendOp;
            Uint32 colorWriteMask;
        };

        std::vector<RenderTargetBlendDesc> renderTargetBlends;
    };

    struct RenderPassDesc {
        struct AttachmentDescription {
            Format format;       // Format of the attachment (e.g., RGBA8, Depth32F)
            LoadOp loadOp;       // Load operation (e.g., Load, Clear)
            StoreOp storeOp;     // Store operation (e.g., Store, DontCare)
            bool isDepthStencil; // Is this attachment used for depth/stencil?
            SampleCount sampleCount; // Multisample count (e.g., 1, 4, 8, 16)
        };

        struct SubpassDescription {
            std::vector<Uint32> colorAttachments;  // Indices of color attachments
            Int32 depthStencilAttachment;         // Index of depth/stencil attachment (-1 if none)
            std::vector<Uint32> inputAttachments; // Attachments used as inputs
            std::vector<Uint32> preserveAttachments; // Attachments preserved for future subpasses
            std::vector<Uint32> resolveAttachments; // Attachments for resolving multisample targets
        };

        struct SubpassDependency {
            Uint32 srcSubpass;        // Source subpass index
            Uint32 dstSubpass;        // Destination subpass index
            PipelineStage srcStage;   // Source pipeline stage (e.g., FragmentShader, Transfer)
            PipelineStage dstStage;   // Destination pipeline stage
            AccessFlags srcAccessMask; // Source access flags (e.g., read/write permissions)
            AccessFlags dstAccessMask; // Destination access flags
        };

        std::vector<AttachmentDescription> attachments; // All attachments used in the render pass
        std::vector<SubpassDescription> subpasses;      // Subpasses within the render pass
        std::vector<SubpassDependency> dependencies;    // Subpass dependencies
    };

    struct ShaderDesc {
        ShaderStage stage;        // Stage of the shader (Vertex, Fragment, Compute, etc.)
        const char *code;         // Pointer to shader source code or binary
        size_t codeSize;          // Size of the shader code in bytes
        const char *entryPoint;   // Entry point name (required for SPIR-V/Vulkan-like APIs)
        const char *debugName;    // Optional name for debugging purposes

        // Optional fields for compile-time settings (if applicable)
        std::vector<std::string> macros;   // List of preprocessor macros (e.g., #define NAME VALUE)
        bool optimize = true;              // Enable/disable compiler optimizations
        bool debugInfo = false;            // Include debug information for shader
    };

    struct InputLayoutDesc {
        // Describes a single vertex buffer binding
        struct InputBindingDesc {
            Uint32 binding;          // Binding index in the shader
            Uint32 stride;           // Size of each vertex in the buffer (in bytes)
            bool inputRateInstance;  // true for per-instance data, false for per-vertex data
        };

        // Describes a single vertex attribute
        struct InputAttributeDesc {
            Uint32 location;         // Location in the vertex shader (layout(location))
            Format format;           // Data format (e.g., RGB32F, RGBA8)
            Uint32 offset;           // Offset within a single vertex (in bytes)
            Uint32 binding;          // The binding index this attribute is sourced from
        };

        // Arrays of bindings and attributes
        std::vector<InputBindingDesc> bindings;
        std::vector<InputAttributeDesc> attributes;
    };

    struct PipelineLayoutDesc {
        struct DescriptorSetLayoutBinding {
            Uint32 binding;         // Binding index in the shader
            DescriptorType type;    // Resource type (e.g., UniformBuffer, SampledImage)
            Uint32 count;           // Number of resources in the binding
            ShaderStage stageFlags; // Shader stages where the resource is used
        };

        // Descriptor sets in this pipeline layout
        struct DescriptorSetLayoutDesc {
            std::vector<DescriptorSetLayoutBinding> bindings; // Bindings within this set
        };

        std::vector<DescriptorSetLayoutDesc> descriptorSets; // All descriptor sets for the pipeline

        Uint32 pushConstantSize;   // Size of push constants (optional, for Vulkan/DirectX-like APIs)
        ShaderStage pushConstantStages; // Stages using push constants (optional)

        const char *debugName;     // Optional name for debugging purposes
    };

    class IShader;

    struct ShaderStages {
        IShader *vertexShader;
        IShader *fragmentShader;
        IShader *computeShader;
        IShader *geometryShader;
        IShader *tessellationControlShader;
        IShader *tessellationEvaluationShader;
    };

    struct RenderStateDesc {
        RasterizationStateDesc rasterizationState;
        DepthStencilStateDesc depthStencilState;
        BlendStateDesc blendState;
    };

    struct PipelineDesc {
        ShaderStages shaders;          // Shaders used in the pipeline
        InputLayoutDesc inputLayout;   // Vertex input layout
        RenderStateDesc renderStates;  // Rasterization, blending, and depth-stencil states
        PipelineLayoutDesc pipelineLayout; // Descriptor set and push constant layout
        RenderPassDesc renderPass;     // Associated render pass
        const char *debugName;         // Optional debug name
    };

    struct ClearValue {
        union {
            float color[4];   // Clear color for color attachments
            struct {
                float depth;  // Clear depth value
                Uint32 stencil; // Clear stencil value
            } depthStencil;
        };
    };

    struct CommandBufferDesc {
        bool isPrimary;       // True if this is a primary command buffer; false for secondary
        bool isGraphics;       // Indicates whether the buffer will record graphics commands.
        bool isCompute;       // Indicates whether the buffer will record compute commands.
        bool allowSimultaneousUse; // True if this buffer can be submitted multiple times concurrently
        const char *debugName;     // Optional name for debugging and profiling
    };

    struct SwapchainDesc {
        Uint32 width;            // Width of the swapchain images
        Uint32 height;           // Height of the swapchain images
        Format format;           // Pixel format of the images
        Uint32 imageCount;       // Number of images in the swapchain
        PresentMode presentMode; // Presentation mode (e.g., FIFO, Mailbox, Immediate)
    };

    // Interfaces
    class IResource {
    public:
        virtual ~IResource() = default;

        virtual ResourceType GetResourceType() const = 0;

        virtual const char *GetDebugName() const = 0;

        virtual void SetDebugName(const char *name) = 0;
    };

    class IBuffer : public IResource {
    public:
        virtual void *Map() = 0;

        virtual void Unmap() = 0;

        virtual size_t GetSize() const = 0;
    };

    class ITexture : public IResource {
    public:
        ~ITexture() override = default;

        virtual Uint32 GetWidth() const = 0;

        virtual Uint32 GetHeight() const = 0;

        virtual Uint32 GetDepth() const = 0;

        virtual Format GetFormat() const = 0;

        virtual Uint32 GetMipLevels() const = 0;
    };

    class ISampler {
    public:
        virtual ~ISampler() = default;
    };

    class IShader {
    public:
        virtual ~IShader() = default;

        virtual ShaderStage GetStage() const = 0;

        virtual const char *GetDebugName() const = 0;
    };

    class IDescriptorSet {
    public:
        virtual ~IDescriptorSet() = default;

        virtual void UpdateBindings(const std::vector<DescriptorBinding> &bindings) = 0;

        virtual void ResetBindings() = 0; // Reset all bindings

        virtual const DescriptorBinding *GetBinding(Uint32 bindingIndex) const = 0; //
    };

    class ICommandBuffer {
    public:
        virtual ~ICommandBuffer() = default;

        virtual void Begin() = 0;

        virtual void End() = 0;

        virtual void Submit() = 0;

        virtual void Reset() = 0; // Reset the command buffer for reuse

        virtual bool IsRecording() const = 0; // Query if the buffer is currently recording
    };

    class ISemaphore {
    public:
        virtual ~ISemaphore() = default;

        // Wait for the semaphore to be signaled
        virtual void Wait(Uint64 timeout = UINT64_MAX) = 0;

        // Check if the semaphore is signaled
        virtual bool IsSignaled() const = 0;

        // Signal the semaphore manually (optional, useful for CPU-GPU sync)
        virtual void Signal() = 0;
    };

    class ITimelineSemaphore : public ISemaphore {
    public:
        // Wait for the semaphore to reach or exceed the specified value
        virtual void WaitForValue(Uint64 value, Uint64 timeout = UINT64_MAX) = 0;

        // Signal the semaphore with the specified value
        virtual void SignalValue(Uint64 value) = 0;

        // Get the current value of the semaphore
        virtual Uint64 GetCurrentValue() const = 0;
    };

    class IFence {
    public:
        virtual ~IFence() = default;

        // Wait until the fence is signaled or timeout expires
        virtual void Wait(Uint64 timeout = UINT64_MAX) = 0;

        // Check if the fence is signaled without blocking
        virtual bool IsSignaled() const = 0;

        // Reset the fence to an unsignaled state
        virtual void Reset() = 0;
    };

    class ISwapchain {
    public:
        virtual ~ISwapchain() = default;

        // Get the properties of the swapchain
        virtual Uint32 GetWidth() const = 0;    // Width of the swapchain images

        virtual Uint32 GetHeight() const = 0;   // Height of the swapchain images

        virtual Format GetFormat() const = 0;   // Format of the swapchain images

        virtual Uint32 GetImageCount() const = 0;

        // Present the current frame to the screen
        virtual void Present(ISemaphore *waitSemaphore = nullptr) = 0;

        // Retrieve the current back buffer for rendering
        virtual ITexture *GetCurrentImage() = 0;

        // Recreate the swapchain in case of resize or other events
        virtual void Recreate(Uint32 width, Uint32 height, Format format) = 0;
    };


    class IPipeline {
    public:
        virtual ~IPipeline() = default;

        virtual const PipelineDesc &GetDescription() const = 0;

        virtual IShader *GetShader(ShaderStage stage) const = 0;
    };

    class IRenderPass {
    public:
        virtual ~IRenderPass() = default;

        // Returns the description of the render pass
        virtual const RenderPassDesc &GetDescription() const = 0;

        // Begin the render pass
        virtual void Begin(ICommandBuffer *commandBuffer, const std::vector<ClearValue> &clearValues) = 0;

        // End the render pass
        virtual void End(ICommandBuffer *commandBuffer) = 0;

        virtual bool IsCompatibleWith(const IPipeline *pipeline) const = 0;
    };


    class IDevice {
    public:
        virtual ~IDevice() = default;

        // Resource creation and management
        virtual IBuffer *CreateBuffer(const BufferDesc &desc) = 0;

        virtual ITexture *CreateTexture(const TextureDesc &desc) = 0;

        virtual ISampler *CreateSampler(const SamplerDesc &desc) = 0;

        virtual IShader *CreateShader(const ShaderDesc &desc) = 0;

        virtual IPipeline *CreatePipeline(const PipelineDesc &desc) = 0;

        virtual IRenderPass *CreateRenderPass(const RenderPassDesc &desc) = 0;

        virtual void DestroyResource(IResource *resource) = 0; // Explicitly destroy a resource

        // Command buffer creation
        virtual ICommandBuffer *CreateCommandBuffer(const CommandBufferDesc &desc) = 0;

        // Swapchain management
        virtual ISwapchain *CreateSwapchain(const SwapchainDesc &desc) = 0;

        // Synchronization objects
        virtual IFence *CreateFence() = 0;

        virtual ISemaphore *CreateSemaphore() = 0;

        virtual ITimelineSemaphore *CreateTimelineSemaphore() = 0;

        // Device-wide synchronization
        virtual void DeviceBarrier(PipelineStage srcStage, PipelineStage dstStage, AccessFlags srcAccess,
                                   AccessFlags dstAccess) = 0;

        // Device capabilities
        virtual bool IsFeatureSupported(DeviceFeature feature) const = 0;

        // Error handling
        virtual DeviceErrorType GetLastErrorType() const = 0; // Retrieve error type

        virtual void ClearLastError() = 0;

        // Debugging
        virtual void EnableDebugLayer(bool enable) = 0;
    };

} // namespace Bcg::Graphics

#endif // ENGINE25_GRAPHICS_API_H
