// This is a conceptual example of a handle-based, backend-agnostic graphics API design.
// It demonstrates enums, descriptors, handle types, and a Device interface that relies on
// a backend-specific function table. This code is not tied to a specific backend implementation.
// To use it in practice, you would implement the backend functions (e.g., OpenGL, Vulkan)
// and provide them to the Device constructor.
//
// Note on naming conventions:
// - Free functions use CamelCase
// - Member variables and local variables use snake_case
// - Types (classes, structs, enums) use PascalCase

#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace Bcg::Graphics {
    // ----------------------------------------------------------
    // Basic Type Aliases
    // ----------------------------------------------------------
    using Int32 = int;
    using Uint32 = unsigned int;
    using Uint64 = unsigned long long;
    using SizeType = size_t;

    // ----------------------------------------------------------
    // Handle Types (Opaque Identifiers)
    // ----------------------------------------------------------
    struct BufferHandle {
        Uint32 id = 0;
    };

    struct TextureHandle {
        Uint32 id = 0;
    };

    struct SamplerHandle {
        Uint32 id = 0;
    };

    struct ShaderHandle {
        Uint32 id = 0;
    };

    struct PipelineHandle {
        Uint32 id = 0;
    };

    struct RenderPassHandle {
        Uint32 id = 0;
    };

    struct FramebufferHandle {
        Uint32 id = 0;
    };

    struct DescriptorSetLayoutHandle {
        Uint32 id = 0;
    };

    struct DescriptorSetHandle {
        Uint32 id = 0;
    };

    struct CommandBufferHandle {
        Uint32 id = 0;
    };

    struct FenceHandle {
        Uint32 id = 0;
    };

    struct SemaphoreHandle {
        Uint32 id = 0;
    };

    struct SwapchainHandle {
        Uint32 id = 0;
    };

    // A helper to indicate invalid handles
    static constexpr Uint32 InvalidId = 0;


    // ----------------------------------------------------------
    // Enums
    // ----------------------------------------------------------
    enum class Format {
        R8,
        RG8,
        RGB8,
        RGBA8,
        RGBA16F,
        RGBA32F,
        Depth24Stencil8,
        Depth32F
    };

    enum class ShaderStage {
        Vertex,
        Fragment,
        Compute,
        Geometry,
        TessellationControl,
        TessellationEvaluation
    };

    enum class Filter {
        Nearest,
        Linear
    };

    enum class MipmapMode {
        Nearest,
        Linear
    };

    enum class AddressMode {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder
    };

    enum class BorderColor {
        TransparentBlack,
        OpaqueBlack,
        OpaqueWhite
    };

    enum class CompareOp {
        Never,
        Less,
        Equal,
        LessOrEqual,
        Greater,
        NotEqual,
        GreaterOrEqual,
        Always
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
        OneMinusDstAlpha
    };

    enum class BlendOp {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    enum class PolygonMode {
        Fill,
        Line,
        Point
    };

    enum class CullMode {
        None,
        Front,
        Back,
        FrontAndBack
    };

    enum class FrontFace {
        Clockwise,
        CounterClockwise
    };

    enum class LoadOp {
        Load,
        Clear,
        DontCare
    };

    enum class StoreOp {
        Store,
        DontCare
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
        StorageBuffer
    };

    enum class TextureUsage {
        Sampled,
        Storage,
        RenderTarget,
        DepthStencil
    };

    enum class PresentMode {
        Immediate,
        Mailbox,
        FIFO
    };

    enum class SampleCount {
        Count1 = 1,
        Count2 = 2,
        Count4 = 4,
        Count8 = 8,
        Count16 = 16
    };

    enum class PipelineStage {
        TopOfPipe,
        VertexInput,
        VertexShader,
        FragmentShader,
        ComputeShader,
        Transfer,
        BottomOfPipe,
        Host
    };

    enum class AccessFlags : Uint32 {
        None = 0,
        Read = 1 << 0,
        Write = 1 << 1
    };

    inline AccessFlags operator|(AccessFlags a, AccessFlags b) {
        return static_cast<AccessFlags>(static_cast<Uint32>(a) | static_cast<Uint32>(b));
    }

    enum class DeviceFeature {
        MultiDrawIndirect,
        ComputeShaders,
        GeometryShaders,
        RayTracing
    };

    enum class DeviceErrorType {
        None,
        OutOfMemory,
        InvalidOperation,
        ResourceCreationFailed,
        DeviceLost,
        Unknown
    };


    // ----------------------------------------------------------
    // Descriptors
    // ----------------------------------------------------------

    struct BufferDesc {
        SizeType size = 0;
        MemoryUsage memory_usage = MemoryUsage::GpuOnly;
        BufferUsage usage = BufferUsage::VertexBuffer;
    };

    struct TextureDesc {
        Uint32 width = 0;
        Uint32 height = 0;
        Uint32 depth = 1;
        Uint32 mip_levels = 1;
        Format format = Format::RGBA8;
        TextureUsage usage = TextureUsage::Sampled;
        // For simplicity, basic sampler-like states (some backends treat these separately)
        Filter min_filter = Filter::Linear;
        Filter mag_filter = Filter::Linear;
        AddressMode address_mode_u = AddressMode::Repeat;
        AddressMode address_mode_v = AddressMode::Repeat;
        AddressMode address_mode_w = AddressMode::Repeat;
    };

    struct SamplerDesc {
        Filter min_filter = Filter::Linear;
        Filter mag_filter = Filter::Linear;
        MipmapMode mipmap_mode = MipmapMode::Linear;
        AddressMode address_mode_u = AddressMode::Repeat;
        AddressMode address_mode_v = AddressMode::Repeat;
        AddressMode address_mode_w = AddressMode::Repeat;
        float mip_lod_bias = 0.0f;
        float max_anisotropy = 1.0f;
        BorderColor border_color = BorderColor::OpaqueBlack;
    };

    struct ShaderDesc {
        ShaderStage stage;
        const void *code = nullptr;
        SizeType code_size = 0;
        const char *entry_point = "main";
        std::vector<std::string> macros;
        bool optimize = true;
        bool debug_info = false;
    };

    struct InputBindingDesc {
        Uint32 binding = 0;
        Uint32 stride = 0;
        bool input_rate_instance = false;
    };

    struct InputAttributeDesc {
        Uint32 location = 0;
        Format format = Format::RGBA8;
        Uint32 offset = 0;
        Uint32 binding = 0;
    };

    struct InputLayoutDesc {
        std::vector<InputBindingDesc> bindings;
        std::vector<InputAttributeDesc> attributes;
    };

    struct RasterizationStateDesc {
        bool depth_clamp_enable = false;
        PolygonMode fill_mode = PolygonMode::Fill;
        CullMode cull_mode = CullMode::Back;
        FrontFace front_face = FrontFace::CounterClockwise;
        bool depth_bias_enable = false;
        float depth_bias_constant_factor = 0.0f;
        float depth_bias_clamp = 0.0f;
        float depth_bias_slope_factor = 0.0f;
    };

    struct DepthStencilStateDesc {
        bool depth_test_enable = true;
        bool depth_write_enable = true;
        CompareOp depth_compare_op = CompareOp::Less;
        bool stencil_test_enable = false;

        struct StencilOpDesc {
            BlendOp fail_op = BlendOp::Add; // Not correct semantically, placeholder for stencil ops
            BlendOp pass_op = BlendOp::Add; // Similarly placeholders
            BlendOp depth_fail_op = BlendOp::Add;
            CompareOp compare_op = CompareOp::Always;
            Uint32 compare_mask = 0xFF;
            Uint32 write_mask = 0xFF;
            Uint32 reference = 0;
        };

        StencilOpDesc front;
        StencilOpDesc back;
    };

    struct BlendStateDesc {
        struct RenderTargetBlendDesc {
            bool blend_enable = false;
            BlendFactor src_color_blend_factor = BlendFactor::One;
            BlendFactor dst_color_blend_factor = BlendFactor::Zero;
            BlendOp color_blend_op = BlendOp::Add;
            BlendFactor src_alpha_blend_factor = BlendFactor::One;
            BlendFactor dst_alpha_blend_factor = BlendFactor::Zero;
            BlendOp alpha_blend_op = BlendOp::Add;
            Uint32 color_write_mask = 0xF; // RGBA
        };

        std::vector<RenderTargetBlendDesc> render_target_blends;
    };

    struct DescriptorSetLayoutBinding {
        Uint32 binding = 0;
        // In a more complex system, you'd have a DescriptorType enum
        // For simplicity, assume uniform buffer binding
        ShaderStage stage_flags = ShaderStage::Vertex;
        Uint32 count = 1;
    };

    struct DescriptorSetLayoutDesc {
        std::vector<DescriptorSetLayoutBinding> bindings;
    };

    struct PipelineLayoutDesc {
        std::vector<DescriptorSetLayoutDesc> descriptor_sets;
        Uint32 push_constant_size = 0;
        ShaderStage push_constant_stages = ShaderStage::Vertex;
    };

    struct RenderPassDesc {
        struct AttachmentDescription {
            Format format;
            LoadOp load_op;
            StoreOp store_op;
            bool is_depth_stencil = false;
            SampleCount sample_count = SampleCount::Count1;
        };

        struct SubpassDescription {
            std::vector<Uint32> color_attachments;
            Int32 depth_stencil_attachment = -1;
        };

        std::vector<AttachmentDescription> attachments;
        std::vector<SubpassDescription> subpasses;
    };

    struct PipelineDesc {
        ShaderHandle vertex_shader;
        ShaderHandle fragment_shader;
        // Add other shader stages as needed

        InputLayoutDesc input_layout;
        RasterizationStateDesc rasterization_state;
        DepthStencilStateDesc depth_stencil_state;
        BlendStateDesc blend_state;
        PipelineLayoutDesc pipeline_layout;
        RenderPassHandle render_pass;
    };

    struct CommandBufferDesc {
        bool is_primary = true;
        bool is_graphics = true;
        bool is_compute = false;
        bool allow_simultaneous_use = false;
    };

    struct SwapchainDesc {
        Uint32 width;
        Uint32 height;
        Format format;
        Uint32 image_count;
        PresentMode present_mode;
    };


    // ----------------------------------------------------------
    // Device Functions Table
    // ----------------------------------------------------------
    struct DeviceFunctions {
        // Resource creation
        BufferHandle (*CreateBuffer)(void *ctx, const BufferDesc &) = nullptr;

        void (*DestroyBuffer)(void *ctx, BufferHandle) = nullptr;

        TextureHandle (*CreateTexture)(void *ctx, const TextureDesc &) = nullptr;

        void (*DestroyTexture)(void *ctx, TextureHandle) = nullptr;

        SamplerHandle (*CreateSampler)(void *ctx, const SamplerDesc &) = nullptr;

        void (*DestroySampler)(void *ctx, SamplerHandle) = nullptr;

        ShaderHandle (*CreateShader)(void *ctx, const ShaderDesc &) = nullptr;

        void (*DestroyShader)(void *ctx, ShaderHandle) = nullptr;

        PipelineHandle (*CreatePipeline)(void *ctx, const PipelineDesc &) = nullptr;

        void (*DestroyPipeline)(void *ctx, PipelineHandle) = nullptr;

        RenderPassHandle (*CreateRenderPass)(void *ctx, const RenderPassDesc &) = nullptr;

        void (*DestroyRenderPass)(void *ctx, RenderPassHandle) = nullptr;

        CommandBufferHandle (*CreateCommandBuffer)(void *ctx, const CommandBufferDesc &) = nullptr;

        void (*DestroyCommandBuffer)(void *ctx, CommandBufferHandle) = nullptr;

        FenceHandle (*CreateFence)(void *ctx) = nullptr;

        void (*DestroyFence)(void *ctx, FenceHandle) = nullptr;

        SemaphoreHandle (*CreateSemaphore)(void *ctx) = nullptr;

        void (*DestroySemaphore)(void *ctx, SemaphoreHandle) = nullptr;

        SwapchainHandle (*CreateSwapchain)(void *ctx, const SwapchainDesc &) = nullptr;

        void (*DestroySwapchain)(void *ctx, SwapchainHandle) = nullptr;

        // Operations
        void (*DeviceBarrier)(void *ctx,
                              PipelineStage src_stage,
                              PipelineStage dst_stage,
                              AccessFlags src_access,
                              AccessFlags dst_access) = nullptr;

        bool (*IsFeatureSupported)(void *ctx, DeviceFeature feature) = nullptr;

        DeviceErrorType (*GetLastErrorType)(void *ctx) = nullptr;

        void (*ClearLastError)(void *ctx) = nullptr;
    };


    // ----------------------------------------------------------
    // Device Class
    // ----------------------------------------------------------
    class Device {
    public:
        Device(const DeviceFunctions &funcs, void *backend_context)
            : funcs_(funcs), backend_context_(backend_context) {
        }

        BufferHandle CreateBuffer(const BufferDesc &desc) {
            return funcs_.CreateBuffer(backend_context_, desc);
        }

        void DestroyBuffer(BufferHandle handle) {
            funcs_.DestroyBuffer(backend_context_, handle);
        }

        TextureHandle CreateTexture(const TextureDesc &desc) {
            return funcs_.CreateTexture(backend_context_, desc);
        }

        void DestroyTexture(TextureHandle handle) {
            funcs_.DestroyTexture(backend_context_, handle);
        }

        SamplerHandle CreateSampler(const SamplerDesc &desc) {
            return funcs_.CreateSampler(backend_context_, desc);
        }

        void DestroySampler(SamplerHandle handle) {
            funcs_.DestroySampler(backend_context_, handle);
        }

        ShaderHandle CreateShader(const ShaderDesc &desc) {
            return funcs_.CreateShader(backend_context_, desc);
        }

        void DestroyShader(ShaderHandle handle) {
            funcs_.DestroyShader(backend_context_, handle);
        }

        PipelineHandle CreatePipeline(const PipelineDesc &desc) {
            return funcs_.CreatePipeline(backend_context_, desc);
        }

        void DestroyPipeline(PipelineHandle handle) {
            funcs_.DestroyPipeline(backend_context_, handle);
        }

        RenderPassHandle CreateRenderPass(const RenderPassDesc &desc) {
            return funcs_.CreateRenderPass(backend_context_, desc);
        }

        void DestroyRenderPass(RenderPassHandle handle) {
            funcs_.DestroyRenderPass(backend_context_, handle);
        }

        CommandBufferHandle CreateCommandBuffer(const CommandBufferDesc &desc) {
            return funcs_.CreateCommandBuffer(backend_context_, desc);
        }

        void DestroyCommandBuffer(CommandBufferHandle handle) {
            funcs_.DestroyCommandBuffer(backend_context_, handle);
        }

        FenceHandle CreateFence() {
            return funcs_.CreateFence(backend_context_);
        }

        void DestroyFence(FenceHandle handle) {
            funcs_.DestroyFence(backend_context_, handle);
        }

        SemaphoreHandle CreateSemaphore() {
            return funcs_.CreateSemaphore(backend_context_);
        }

        void DestroySemaphore(SemaphoreHandle handle) {
            funcs_.DestroySemaphore(backend_context_, handle);
        }

        SwapchainHandle CreateSwapchain(const SwapchainDesc &desc) {
            return funcs_.CreateSwapchain(backend_context_, desc);
        }

        void DestroySwapchain(SwapchainHandle handle) {
            funcs_.DestroySwapchain(backend_context_, handle);
        }

        void DeviceBarrier(PipelineStage src_stage,
                           PipelineStage dst_stage,
                           AccessFlags src_access,
                           AccessFlags dst_access) {
            funcs_.DeviceBarrier(backend_context_, src_stage, dst_stage, src_access, dst_access);
        }

        bool IsFeatureSupported(DeviceFeature feature) const {
            return funcs_.IsFeatureSupported(backend_context_, feature);
        }

        DeviceErrorType GetLastErrorType() const {
            return funcs_.GetLastErrorType(backend_context_);
        }

        void ClearLastError() {
            funcs_.ClearLastError(backend_context_);
        }

    private:
        DeviceFunctions funcs_;
        void *backend_context_ = nullptr;
    };


    // ----------------------------------------------------------
    // Example RAII Wrapper (for illustration)
    // ----------------------------------------------------------
    class Buffer {
    public:
        Buffer(Device &device, const BufferDesc &desc)
            : device_(device) {
            handle_ = device_.CreateBuffer(desc);
        }

        ~Buffer() {
            if (handle_.id != InvalidId) {
                device_.DestroyBuffer(handle_);
            }
        }

        BufferHandle GetHandle() const { return handle_; }

    private:
        Device &device_;
        BufferHandle handle_{};
    };

    // Similarly, you could create RAII wrappers for other resources.


    // ----------------------------------------------------------
    // This completes the conceptual code example.
    // ----------------------------------------------------------
}
