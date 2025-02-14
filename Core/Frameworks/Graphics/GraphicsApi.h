#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>

namespace Bcg::Graphics {
    using Int32 = int;
    using Uint32 = unsigned int;
    using Uint64 = unsigned long long;
    using SizeType = size_t;

    // ----------------------------------------------------------
    // Handle Types (Opaque Identifiers)
    //
    // These are lightweight identifiers for GPU resources.
    // They do not directly own or manage the underlying resources.
    // Creation and destruction are handled by the Device and its backend.
    // ----------------------------------------------------------
    enum class BackendType {
        Unknown,
        Vulkan,
        OpenGL,
        Direct3D12,
        Metal
    };

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

    static constexpr Uint32 InvalidId = 0;

    // ----------------------------------------------------------
    // Enums
    //
    // These enums describe various aspects of rendering state,
    // resource formats, pipeline stages, etc. They aim to be
    // backend-agnostic, with enough detail for a high-end renderer.
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
    // Descriptors for Resource Creation
    //
    // These structs define how to create various GPU resources.
    // They are passed to the Device's create functions, and must
    // be compatible with backend capabilities.
    //
    // Example usage:
    //   BufferDesc bd = {...};
    //   BufferHandle bh = device.create_buffer(bd);
    //   // Use bh in rendering operations.
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
        const void *code = nullptr;     // SPIR-V, DXBC, DXIL, or backend-specific bytecode
        SizeType code_size = 0;
        const char *entry_point = "main";
        std::vector<std::string> macros;
        bool optimize = true;
        bool debug_info = false;
    };

    struct InputBindingDesc {
        Uint32 binding = 0;
        Uint32 stride = 0;
        bool input_rate_instance = false; // true if per-instance data, false if per-vertex
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
            // Placeholders for stencil ops, adapt as needed
            BlendOp fail_op = BlendOp::Add;
            BlendOp pass_op = BlendOp::Add;
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
        ShaderStage stage_flags = ShaderStage::Vertex;
        Uint32 count = 1;
        // The descriptor type can be inferred at DescriptorSet creation,
        // or extended to be part of the layout if desired.
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

    enum class DescriptorType {
        UniformBuffer,
        StorageBuffer,
        CombinedImageSampler,
        Sampler,
        SampledImage,
        StorageImage
    };

    struct DescriptorBinding {
        Uint32 binding = 0;
        DescriptorType type = DescriptorType::UniformBuffer;
        BufferHandle buffer = {};
        SizeType buffer_offset = 0;
        SizeType buffer_range = 0;
        TextureHandle texture = {};
        SamplerHandle sampler = {};
    };

    struct FramebufferAttachmentDesc {
        TextureHandle texture;
        Uint32 mip_level = 0;
        Uint32 base_layer = 0;
        Uint32 layer_count = 1;
    };

    struct FramebufferDesc {
        RenderPassHandle render_pass;
        std::vector<FramebufferAttachmentDesc> attachments;
        Uint32 width = 0;
        Uint32 height = 0;
        Uint32 layers = 1;
    };

    struct DescriptorSetDesc {
        DescriptorSetLayoutHandle layout;
        std::vector<DescriptorBinding> bindings;
    };

    // ----------------------------------------------------------
    // Additional Operations for a High-End Renderer:
    //
    // To properly manage synchronization, command submission,
    // and presentation, we add functions to the device that:
    // - Begin/End command buffer recording
    // - Update existing descriptor sets (if supported by the backend)
    // - submit command buffers to queues
    // - Wait for fences and reset them
    // - Acquire images from the swapchain and present them
    // ----------------------------------------------------------

    struct SubmitInfo {
        std::vector<CommandBufferHandle> command_buffers;
        std::vector<SemaphoreHandle> wait_semaphores;   // Semaphores to wait on before execution
        std::vector<SemaphoreHandle> signal_semaphores; // Semaphores to signal after execution
        FenceHandle fence = {}; // Optional fence to signal when done
    };

    struct PresentInfo {
        SwapchainHandle swapchain;
        Uint32 image_index;
        std::vector<SemaphoreHandle> wait_semaphores;
    };

    // Update info for descriptors after creation:
    struct DescriptorUpdate {
        DescriptorSetHandle descriptor_set;
        DescriptorBinding binding_info;
    };

    // ----------------------------------------------------------
    // Device Functions Table
    //
    // This structure holds function pointers for all operations.
    // A backend implementation sets these pointers to functions that
    // carry out the actual work. The Device class then calls them.
    // ----------------------------------------------------------
    struct DeviceFunctions {
        // Resource creation/destruction
        BufferHandle (*create_buffer)(void *ctx, const BufferDesc &) = nullptr;

        void (*destroy_buffer)(void *ctx, BufferHandle) = nullptr;

        TextureHandle (*create_texture)(void *ctx, const TextureDesc &) = nullptr;

        void (*destroy_texture)(void *ctx, TextureHandle) = nullptr;

        SamplerHandle (*create_sampler)(void *ctx, const SamplerDesc &) = nullptr;

        void (*destroy_sampler)(void *ctx, SamplerHandle) = nullptr;

        ShaderHandle (*create_shader)(void *ctx, const ShaderDesc &) = nullptr;

        void (*destroy_shader)(void *ctx, ShaderHandle) = nullptr;

        PipelineHandle (*create_pipeline)(void *ctx, const PipelineDesc &) = nullptr;

        void (*destroy_pipeline)(void *ctx, PipelineHandle) = nullptr;

        RenderPassHandle (*create_render_pass)(void *ctx, const RenderPassDesc &) = nullptr;

        void (*destroy_render_pass)(void *ctx, RenderPassHandle) = nullptr;

        FramebufferHandle (*create_framebuffer)(void *ctx, const FramebufferDesc &) = nullptr;

        void (*destroy_framebuffer)(void *ctx, FramebufferHandle) = nullptr;

        DescriptorSetLayoutHandle (*create_descriptor_set_layout)(void *ctx, const DescriptorSetLayoutDesc &) = nullptr;

        void (*destroy_descriptor_set_layout)(void *ctx, DescriptorSetLayoutHandle) = nullptr;

        DescriptorSetHandle (*create_descriptor_set)(void *ctx, const DescriptorSetDesc &) = nullptr;

        void (*destroy_descriptor_set)(void *ctx, DescriptorSetHandle) = nullptr;

        CommandBufferHandle (*create_command_buffer)(void *ctx, const CommandBufferDesc &) = nullptr;

        void (*destroy_command_buffer)(void *ctx, CommandBufferHandle) = nullptr;

        FenceHandle (*create_fence)(void *ctx) = nullptr;

        void (*destroy_fence)(void *ctx, FenceHandle) = nullptr;

        SemaphoreHandle (*create_semaphore)(void *ctx) = nullptr;

        void (*destroy_semaphore)(void *ctx, SemaphoreHandle) = nullptr;

        SwapchainHandle (*create_swapchain)(void *ctx, const SwapchainDesc &) = nullptr;

        void (*destroy_swapchain)(void *ctx, SwapchainHandle) = nullptr;

        // Memory barriers for resource synchronization
        void (*device_barrier)(void *ctx, PipelineStage src_stage, PipelineStage dst_stage,
                              AccessFlags src_access, AccessFlags dst_access) = nullptr;

        bool (*is_feature_supported)(void *ctx, DeviceFeature feature) = nullptr;

        DeviceErrorType (*get_last_error_type)(void *ctx) = nullptr;

        void (*clear_last_error)(void *ctx) = nullptr;

        // High-end features:
        // Command buffer recording
        void (*begin_command_buffer)(void *ctx, CommandBufferHandle) = nullptr;

        void (*end_command_buffer)(void *ctx, CommandBufferHandle) = nullptr;

        // submitting command buffers for execution
        void (*submit)(void *ctx, const SubmitInfo &) = nullptr;

        void
        (*wait_for_fences)(void *ctx, const std::vector<FenceHandle> &fences, bool wait_all, Uint64 timeout) = nullptr;

        void (*reset_fences)(void *ctx, const std::vector<FenceHandle> &fences) = nullptr;

        // Swapchain operations for presentation
        bool
        (*acquire_next_image)(void *ctx, SwapchainHandle, Uint64 timeout, SemaphoreHandle semaphore, FenceHandle fence,
                            Uint32 *image_index) = nullptr;

        void (*present)(void *ctx, const PresentInfo &) = nullptr;

        // Descriptor set updates for changing bound resources on-the-fly
        void (*update_descriptor_sets)(void *ctx, const std::vector<DescriptorUpdate> &) = nullptr;
    };

    // ----------------------------------------------------------
    // Device Class
    //
    // This class provides a C++ interface over the raw function pointers.
    // Users of the API typically create a Device instance with a fully
    // populated DeviceFunctions and a backend context, then use these
    // methods to create and manage resources.
    // ----------------------------------------------------------
    class Device {
    public:
        Device(const DeviceFunctions &funcs, void *backend_context, BackendType backend_type)
                : funcs_(funcs), backend_context_(backend_context), backend_type_(backend_type) {
        }

        // Resource Creation/Destruction
        BufferHandle create_buffer(const BufferDesc &desc) { return funcs_.create_buffer(backend_context_, desc); }

        void destroy_buffer(BufferHandle handle) { funcs_.destroy_buffer(backend_context_, handle); }

        TextureHandle create_texture(const TextureDesc &desc) { return funcs_.create_texture(backend_context_, desc); }

        void destroy_texture(TextureHandle handle) { funcs_.destroy_texture(backend_context_, handle); }

        SamplerHandle create_sampler(const SamplerDesc &desc) { return funcs_.create_sampler(backend_context_, desc); }

        void destroy_sampler(SamplerHandle handle) { funcs_.destroy_sampler(backend_context_, handle); }

        ShaderHandle create_shader(const ShaderDesc &desc) { return funcs_.create_shader(backend_context_, desc); }

        void destroy_shader(ShaderHandle handle) { funcs_.destroy_shader(backend_context_, handle); }

        PipelineHandle create_pipeline(const PipelineDesc &desc) {
            return funcs_.create_pipeline(backend_context_, desc);
        }

        void destroy_pipeline(PipelineHandle handle) { funcs_.destroy_pipeline(backend_context_, handle); }

        RenderPassHandle create_render_pass(const RenderPassDesc &desc) {
            return funcs_.create_render_pass(backend_context_, desc);
        }

        void destroy_render_pass(RenderPassHandle handle) { funcs_.destroy_render_pass(backend_context_, handle); }

        FramebufferHandle create_framebuffer(const FramebufferDesc &desc) {
            return funcs_.create_framebuffer(backend_context_, desc);
        }

        void destroy_framebuffer(FramebufferHandle handle) { funcs_.destroy_framebuffer(backend_context_, handle); }

        DescriptorSetLayoutHandle create_descriptor_set_layout(const DescriptorSetLayoutDesc &desc) {
            return funcs_.create_descriptor_set_layout(backend_context_, desc);
        }

        void destroy_descriptor_set_layout(DescriptorSetLayoutHandle handle) {
            funcs_.destroy_descriptor_set_layout(backend_context_, handle);
        }

        DescriptorSetHandle create_descriptor_set(const DescriptorSetDesc &desc) {
            return funcs_.create_descriptor_set(backend_context_, desc);
        }

        void destroy_descriptor_set(DescriptorSetHandle handle) { funcs_.destroy_descriptor_set(backend_context_, handle); }

        CommandBufferHandle create_command_buffer(const CommandBufferDesc &desc) {
            return funcs_.create_command_buffer(backend_context_, desc);
        }

        void destroy_command_buffer(CommandBufferHandle handle) { funcs_.destroy_command_buffer(backend_context_, handle); }

        FenceHandle create_fence() { return funcs_.create_fence(backend_context_); }

        void destroy_fence(FenceHandle handle) { funcs_.destroy_fence(backend_context_, handle); }

        SemaphoreHandle create_semaphore() { return funcs_.create_semaphore(backend_context_); }

        void destroy_semaphore(SemaphoreHandle handle) { funcs_.destroy_semaphore(backend_context_, handle); }

        SwapchainHandle create_swapchain(const SwapchainDesc &desc) {
            return funcs_.create_swapchain(backend_context_, desc);
        }

        void destroy_swapchain(SwapchainHandle handle) { funcs_.destroy_swapchain(backend_context_, handle); }

        void device_barrier(PipelineStage src_stage, PipelineStage dst_stage, AccessFlags src_access,
                           AccessFlags dst_access) {
            funcs_.device_barrier(backend_context_, src_stage, dst_stage, src_access, dst_access);
        }

        bool is_feature_supported(DeviceFeature feature) const {
            return funcs_.is_feature_supported(backend_context_, feature);
        }

        DeviceErrorType get_last_error_type() const { return funcs_.get_last_error_type(backend_context_); }

        void clear_last_error() { funcs_.clear_last_error(backend_context_); }

        // High-end operations
        void begin_command_buffer(CommandBufferHandle cmd) { funcs_.begin_command_buffer(backend_context_, cmd); }

        void end_command_buffer(CommandBufferHandle cmd) { funcs_.end_command_buffer(backend_context_, cmd); }

        void submit(const SubmitInfo &info) { funcs_.submit(backend_context_, info); }

        void wait_for_fences(const std::vector<FenceHandle> &fences, bool wait_all, Uint64 timeout) {
            funcs_.wait_for_fences(backend_context_, fences, wait_all, timeout);
        }

        void reset_fences(const std::vector<FenceHandle> &fences) {
            funcs_.reset_fences(backend_context_, fences);
        }

        bool acquire_next_image(SwapchainHandle swapchain, Uint64 timeout, SemaphoreHandle semaphore, FenceHandle fence,
                              Uint32 *image_index) {
            return funcs_.acquire_next_image(backend_context_, swapchain, timeout, semaphore, fence, image_index);
        }

        void present(const PresentInfo &info) {
            funcs_.present(backend_context_, info);
        }

        void update_descriptor_sets(const std::vector<DescriptorUpdate> &updates) {
            funcs_.update_descriptor_sets(backend_context_, updates);
        }

        BackendType get_backend_type() const { return backend_type_; }

    private:
        DeviceFunctions funcs_;
        void *backend_context_ = nullptr;
        BackendType backend_type_;
    };



    // ----------------------------------------------------------
    // RAII Wrappers
    //
    // These classes simplify resource management by automatically
    // destroying resources when they go out of scope. Use them to
    // ensure that GPU resources are properly released.
    //
    // Example:
    //   {
    //       Buffer buffer(device, BufferDesc{...});
    //       // Use buffer.get_handle() for rendering
    //   } // buffer is destroyed automatically here
    // ----------------------------------------------------------


    template<typename T, typename Desc, typename Handle>
    class ObjectManager {
    public:
        explicit ObjectManager(Device &device) : device_(device) {}

        // Creates a new Buffer with the given descriptor.
        // Returns the BufferHandle for later retrieval.
        Handle create(const Desc &desc) {
            auto object = std::make_unique<T>(device_, desc);
            Handle handle = object->get_handle();
            objects_.emplace(handle.id, std::move(object));
            return handle;
        }

        // Retrieves a pointer to the Buffer corresponding to the given handle.
        // Returns nullptr if not found.
        T* get(const Handle &handle) {
            auto it = objects_.find(handle.id);
            return (it != objects_.end()) ? it->second.get() : nullptr;
        }

        // Destroys the Buffer corresponding to the given handle.
        void destroy(const Handle &handle) {
            auto it = objects_.find(handle.id);
            if (it != objects_.end()) {
                objects_.erase(it);
            }
        }

        // Optionally, clear all managed buffers.
        void clear() {
            objects_.clear();
        }
    private:
        Device &device_;
        std::unordered_map<unsigned int, std::unique_ptr<T>> objects_;
    };

    class Buffer {
    public:
        Buffer(Device &device, const BufferDesc &desc)
                : device_(device) {
            handle_ = device_.create_buffer(desc);
        }

        ~Buffer() {
            if (handle_.id != InvalidId) {
                device_.destroy_buffer(handle_);
            }
        }

        // Delete copy semantics
        Buffer(const Buffer &) = delete;

        Buffer &operator=(const Buffer &) = delete;

        // Implement move semantics
        Buffer(Buffer &&other) noexcept: device_(other.device_), handle_(other.handle_) {
            other.handle_.id = InvalidId;
        }

        Buffer &operator=(Buffer &&other) noexcept {
            if (this != &other) {
                // Destroy current resource
                if (handle_.id != InvalidId) {
                    device_.destroy_buffer(handle_);
                }
                device_ = other.device_;
                handle_ = other.handle_;
                other.handle_.id = InvalidId;
            }
            return *this;
        }

        [[nodiscard]] BufferHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        BufferHandle handle_{};
    };

    class Texture {
    public:
        Texture(Device &device, const TextureDesc &desc)
                : device_(device) {
            handle_ = device_.create_texture(desc);
        }

        ~Texture() {
            if (handle_.id != InvalidId) {
                device_.destroy_texture(handle_);
            }
        }

        TextureHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        TextureHandle handle_{};
    };

    class Sampler {
    public:
        Sampler(Device &device, const SamplerDesc &desc)
                : device_(device) {
            handle_ = device_.create_sampler(desc);
        }

        ~Sampler() {
            if (handle_.id != InvalidId) {
                device_.destroy_sampler(handle_);
            }
        }

        SamplerHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        SamplerHandle handle_{};
    };

    class Shader {
    public:
        Shader(Device &device, const ShaderDesc &desc)
                : device_(device) {
            handle_ = device_.create_shader(desc);
        }

        ~Shader() {
            if (handle_.id != InvalidId) {
                device_.destroy_shader(handle_);
            }
        }

        ShaderHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        ShaderHandle handle_{};
    };

    class Pipeline {
    public:
        Pipeline(Device &device, const PipelineDesc &desc)
                : device_(device) {
            handle_ = device_.create_pipeline(desc);
        }

        ~Pipeline() {
            if (handle_.id != InvalidId) {
                device_.destroy_pipeline(handle_);
            }
        }

        PipelineHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        PipelineHandle handle_{};
    };

    class RenderPass {
    public:
        RenderPass(Device &device, const RenderPassDesc &desc)
                : device_(device) {
            handle_ = device_.create_render_pass(desc);
        }

        ~RenderPass() {
            if (handle_.id != InvalidId) {
                device_.destroy_render_pass(handle_);
            }
        }

        RenderPassHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        RenderPassHandle handle_{};
    };

    class Framebuffer {
    public:
        Framebuffer(Device &device, const FramebufferDesc &desc)
                : device_(device) {
            handle_ = device_.create_framebuffer(desc);
        }

        ~Framebuffer() {
            if (handle_.id != InvalidId) {
                device_.destroy_framebuffer(handle_);
            }
        }

        FramebufferHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        FramebufferHandle handle_{};
    };

    class DescriptorSetLayout {
    public:
        DescriptorSetLayout(Device &device, const DescriptorSetLayoutDesc &desc)
                : device_(device) {
            handle_ = device_.create_descriptor_set_layout(desc);
        }

        ~DescriptorSetLayout() {
            if (handle_.id != InvalidId) {
                device_.destroy_descriptor_set_layout(handle_);
            }
        }

        DescriptorSetLayoutHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        DescriptorSetLayoutHandle handle_{};
    };

    class DescriptorSet {
    public:
        DescriptorSet(Device &device, const DescriptorSetDesc &desc)
                : device_(device) {
            handle_ = device_.create_descriptor_set(desc);
        }

        ~DescriptorSet() {
            if (handle_.id != InvalidId) {
                device_.destroy_descriptor_set(handle_);
            }
        }

        DescriptorSetHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        DescriptorSetHandle handle_{};
    };

    class CommandBuffer {
    public:
        CommandBuffer(Device &device, const CommandBufferDesc &desc)
                : device_(device) {
            handle_ = device_.create_command_buffer(desc);
        }

        ~CommandBuffer() {
            if (handle_.id != InvalidId) {
                device_.destroy_command_buffer(handle_);
            }
        }

        CommandBufferHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        CommandBufferHandle handle_{};
    };

    class Fence {
    public:
        Fence(Device &device)
                : device_(device) {
            handle_ = device_.create_fence();
        }

        ~Fence() {
            if (handle_.id != InvalidId) {
                device_.destroy_fence(handle_);
            }
        }

        FenceHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        FenceHandle handle_{};
    };

    class Semaphore {
    public:
        Semaphore(Device &device)
                : device_(device) {
            handle_ = device_.create_semaphore();
        }

        ~Semaphore() {
            if (handle_.id != InvalidId) {
                device_.destroy_semaphore(handle_);
            }
        }

        SemaphoreHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        SemaphoreHandle handle_{};
    };

    class Swapchain {
    public:
        Swapchain(Device &device, const SwapchainDesc &desc)
                : device_(device) {
            handle_ = device_.create_swapchain(desc);
        }

        ~Swapchain() {
            if (handle_.id != InvalidId) {
                device_.destroy_swapchain(handle_);
            }
        }

        SwapchainHandle get_handle() const { return handle_; }

    private:
        Device &device_;
        SwapchainHandle handle_{};
    };

} // namespace Bcg::Graphics
