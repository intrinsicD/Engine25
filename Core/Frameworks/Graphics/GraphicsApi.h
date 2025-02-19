#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "GraphicsTypes.h"

namespace Bcg::Graphics {
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
        Uint64 id = 0;
    };

    struct SemaphoreHandle {
        Uint32 id = 0;
    };

    struct SwapchainHandle {
        Uint32 id = 0;
    };

    struct TextureViewHandle {
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

    enum class TextureType {
        Texture1D,
        Texture2D,
        Texture3D,
        CubeMap,
        TextureArray
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

    enum class DescriptorType {
        UniformBuffer,
        StorageBuffer,
        CombinedImageSampler,
        Sampler,
        SampledImage,
        StorageImage
    };

    // Used for index buffer binding.
    enum class IndexType {
        UInt16,
        UInt32
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
        TextureType type = TextureType::Texture2D;
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
        const void *code = nullptr; // SPIR-V, DXBC, DXIL, or backend-specific bytecode
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
        ShaderHandle geometry_shader = {};
        ShaderHandle tessellation_control_shader = {};
        ShaderHandle tessellation_evaluation_shader = {};
        InputLayoutDesc input_layout;
        RasterizationStateDesc rasterization_state;
        DepthStencilStateDesc depth_stencil_state;
        BlendStateDesc blend_state;
        PipelineLayoutDesc pipeline_layout;
        RenderPassHandle render_pass;
    };

    // Compute Pipeline Descriptor
    struct ComputePipelineDesc {
        ShaderHandle compute_shader;
        PipelineLayoutDesc pipeline_layout;
    };

    // Mesh Pipeline Descriptor
    struct MeshPipelineDesc {
        ShaderHandle mesh_shader;
        ShaderHandle fragment_shader;
        PipelineLayoutDesc pipeline_layout;
        RasterizationStateDesc rasterization_state;
        DepthStencilStateDesc depth_stencil_state;
        BlendStateDesc blend_state;
        RenderPassHandle render_pass;
    };

    // Texture View Descriptor
    struct TextureViewDesc {
        TextureHandle texture;
        Format format; // Allows reinterpretation if needed.
        Uint32 base_mip_level = 0;
        Uint32 level_count = 1;
        Uint32 base_array_layer = 0;
        Uint32 layer_count = 1;
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

    // Used for viewport state.
    struct Viewport {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
        float min_depth = 0.0f;
        float max_depth = 1.0f;
    };

    // Used for scissor state.
    struct Rect2D {
        Int32 x = 0;
        Int32 y = 0;
        Uint32 width = 0;
        Uint32 height = 0;
    };


    struct ClearValue {
        union {
            struct {
                float r, g, b, a;
            } color;
            struct {
                float depth;
                Uint32 stencil;
            } depth_stencil;
        };
    };

    struct RenderPassBeginInfo {
        RenderPassHandle render_pass;          // Render pass handle to use
        FramebufferHandle framebuffer;         // Target framebuffer
        Rect2D render_area;                    // Render area (x, y, width, height)
        std::vector<ClearValue> clear_values;  // Clear values for attachments
    };

    // New handle for pipeline layouts (to be used with push constants).
    struct PipelineLayoutHandle {
        Uint32 id = 0;
    };

    // New handle for query pools.
    struct QueryPoolHandle {
        Uint32 id = 0;
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
        std::vector<SemaphoreHandle> wait_semaphores; // Semaphores to wait on before execution
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
    // Draw*IndirectCommand Structure
    // ----------------------------------------------------------
    struct DrawElementsIndirectCommand {
        Uint32 count; // Number of indices per draw.
        Uint32 instance_count; // Number of instances to draw.
        Uint32 first_index; // Offset in the index buffer.
        Uint32 base_vertex; // Added to each index.
        Uint32 base_instance; // Starting instance ID.
    };

    struct DrawArraysIndirectCommand {
        Uint32 count; // Number of vertices to draw per command.
        Uint32 instance_count; // Number of instances to draw.
        Uint32 first; // Starting vertex offset.
        Uint32 base_instance; // Starting instance ID.
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

        // Command buffer recording
        void (*begin_command_buffer)(void *ctx, CommandBufferHandle) = nullptr;

        void (*end_command_buffer)(void *ctx, CommandBufferHandle) = nullptr;

        // Submitting command buffers
        void (*submit)(void *ctx, const SubmitInfo &) = nullptr;

        void
        (*wait_for_fences)(void *ctx, const std::vector<FenceHandle> &fences, bool wait_all, Uint64 timeout) = nullptr;

        void (*reset_fences)(void *ctx, const std::vector<FenceHandle> &fences) = nullptr;

        // Swapchain operations
        bool
        (*acquire_next_image)(void *ctx, SwapchainHandle, Uint64 timeout, SemaphoreHandle semaphore, FenceHandle fence,
                              Uint32 *image_index) = nullptr;

        void (*present)(void *ctx, const PresentInfo &) = nullptr;

        // Descriptor set updates
        void (*update_descriptor_sets)(void *ctx, const std::vector<DescriptorUpdate> &) = nullptr;

        // Direct draw commands
        void (*draw)(void *ctx, Uint32 vertex_count, Uint32 instance_count, Uint32 first_vertex,
                     Uint32 first_instance) = nullptr;

        void
        (*draw_indexed)(void *ctx, Uint32 index_count, Uint32 instance_count, Uint32 first_index, Int32 vertex_offset,
                        Uint32 first_instance) = nullptr;

        // ----------------------------------------------------------
        // Multi-draw indirect draw call support.
        // The indirect buffer must contain an array of DrawElementsIndirectCommand.
        // ----------------------------------------------------------
        void (*multi_draw_indirect)(void *ctx, Uint32 mode, Uint32 type, const void *indirect, Uint32 draw_count,
                                    Uint32 stride) = nullptr;

        void (*multi_draw_arrays_indirect)(void *ctx, Uint32 mode, const void *indirect, Uint32 draw_count,
                                           Uint32 stride) = nullptr;

        // ----------------------------------------------------------
        // Buffer mapping functions.
        // ----------------------------------------------------------
        void *(*map_buffer)(void *ctx, BufferHandle, Uint32 offset, Uint32 size, AccessFlags flags) = nullptr;

        void (*unmap_buffer)(void *ctx, BufferHandle) = nullptr;

        void (*update_buffer)(void *ctx, BufferHandle, const void *data, Uint32 offset, Uint32 size) = nullptr;

        // ---------------------------
        // Compute dispatch commands
        // ---------------------------
        void (*dispatch)(void *ctx, Uint32 group_count_x, Uint32 group_count_y, Uint32 group_count_z) = nullptr;

        void
        (*dispatch_indirect)(void *ctx, CommandBufferHandle cmd, BufferHandle indirect_buffer, Uint32 offset) = nullptr;

        // ---------------------------
        // Pipeline binding and dynamic state commands
        // ---------------------------
        void (*bind_pipeline)(void *ctx, CommandBufferHandle cmd, PipelineHandle pipeline) = nullptr;

        void
        (*bind_vertex_buffers)(void *ctx, CommandBufferHandle cmd, Uint32 first_binding, const BufferHandle *buffers,
                               const Uint32 *offsets, Uint32 count) = nullptr;

        void (*bind_index_buffer)(void *ctx, CommandBufferHandle cmd, BufferHandle buffer, Uint32 offset,
                                  IndexType index_type) = nullptr;

        void (*set_viewport)(void *ctx, CommandBufferHandle cmd, const Viewport &viewport) = nullptr;

        void (*set_scissor)(void *ctx, CommandBufferHandle cmd, const Rect2D &rect) = nullptr;

        void
        (*update_push_constants)(void *ctx, CommandBufferHandle cmd, PipelineLayoutHandle layout, ShaderStage stage,
                                 Uint32 offset, Uint32 size, const void *data) = nullptr;

        // ---------------------------
        // Resource copy operations
        // ---------------------------
        void (*copy_buffer)(void *ctx, BufferHandle src, BufferHandle dst, Uint32 src_offset, Uint32 dst_offset,
                            Uint32 size) = nullptr;

        void (*copy_texture)(void *ctx, TextureHandle src, TextureHandle dst) = nullptr;

        // ---------------------------
        // Query and debug utilities
        // ---------------------------
        void (*reset_query_pool)(void *ctx, QueryPoolHandle pool, Uint32 first_query, Uint32 query_count) = nullptr;

        void (*begin_query)(void *ctx, CommandBufferHandle cmd, QueryPoolHandle pool, Uint32 query,
                            const char *name) = nullptr;

        void (*end_query)(void *ctx, CommandBufferHandle cmd, QueryPoolHandle pool, Uint32 query) = nullptr;

        void
        (*get_query_results)(void *ctx, QueryPoolHandle pool, Uint32 first_query, Uint32 query_count, void *results,
                             Uint32 stride, bool wait) = nullptr;

        void (*begin_debug_marker)(void *ctx, CommandBufferHandle cmd, const char *marker_name,
                                   const float color[4]) = nullptr;

        void (*end_debug_marker)(void *ctx, CommandBufferHandle cmd) = nullptr;

        void (*insert_debug_marker)(void *ctx, CommandBufferHandle cmd, const char *marker_name,
                                    const float color[4]) = nullptr;

        // ---------------------------
        // Pipeline layout creation/destruction
        // ---------------------------
        PipelineLayoutHandle (*create_pipeline_layout)(void *ctx, const PipelineLayoutDesc &desc) = nullptr;

        void (*destroy_pipeline_layout)(void *ctx, PipelineLayoutHandle layout) = nullptr;

        // ---------------------------
        // Compute Pipeline creation/destruction
        // ---------------------------
        PipelineHandle (*create_compute_pipeline)(void *ctx, const ComputePipelineDesc &) = nullptr;
        void (*destroy_compute_pipeline)(void *ctx, PipelineHandle) = nullptr;

        // ---------------------------
        // Mesh Pipeline creation/destruction
        // ---------------------------
        PipelineHandle (*create_mesh_pipeline)(void *ctx, const MeshPipelineDesc &) = nullptr;
        void (*destroy_mesh_pipeline)(void *ctx, PipelineHandle) = nullptr;

        // Texture view creation/destruction
        TextureViewHandle (*create_texture_view)(void *ctx, const TextureViewDesc &) = nullptr;
        void (*destroy_texture_view)(void *ctx, TextureViewHandle) = nullptr;

        // New: Begin a render pass
        void (*begin_render_pass)(void *ctx, CommandBufferHandle cmd, const RenderPassBeginInfo &begin_info) = nullptr;

        // New: End a render pass
        void (*end_render_pass)(void *ctx, CommandBufferHandle cmd) = nullptr;
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

        void destroy_descriptor_set(DescriptorSetHandle handle) {
            funcs_.destroy_descriptor_set(backend_context_, handle);
        }

        CommandBufferHandle create_command_buffer(const CommandBufferDesc &desc) {
            return funcs_.create_command_buffer(backend_context_, desc);
        }

        void destroy_command_buffer(CommandBufferHandle handle) {
            funcs_.destroy_command_buffer(backend_context_, handle);
        }

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

        [[nodiscard]] bool is_feature_supported(DeviceFeature feature) const {
            return funcs_.is_feature_supported(backend_context_, feature);
        }

        [[nodiscard]] DeviceErrorType get_last_error_type() const {
            return funcs_.get_last_error_type(backend_context_);
        }

        void clear_last_error() { funcs_.clear_last_error(backend_context_); }

        // Command buffer recording
        void begin_command_buffer(CommandBufferHandle cmd) { funcs_.begin_command_buffer(backend_context_, cmd); }

        void end_command_buffer(CommandBufferHandle cmd) { funcs_.end_command_buffer(backend_context_, cmd); }

        // Submitting command buffers
        void submit(const SubmitInfo &info) { funcs_.submit(backend_context_, info); }

        void wait_for_fences(const std::vector<FenceHandle> &fences, bool wait_all, Uint64 timeout) {
            funcs_.wait_for_fences(backend_context_, fences, wait_all, timeout);
        }

        void reset_fences(const std::vector<FenceHandle> &fences) { funcs_.reset_fences(backend_context_, fences); }

        // Swapchain operations
        bool acquire_next_image(SwapchainHandle swapchain, Uint64 timeout, SemaphoreHandle semaphore, FenceHandle fence,
                                Uint32 *image_index) {
            return funcs_.acquire_next_image(backend_context_, swapchain, timeout, semaphore, fence, image_index);
        }

        void present(const PresentInfo &info) { funcs_.present(backend_context_, info); }

        // Descriptor set updates
        void update_descriptor_sets(const std::vector<DescriptorUpdate> &updates) {
            funcs_.update_descriptor_sets(backend_context_, updates);
        }

        // Direct draw commands
        void draw(Uint32 vertex_count, Uint32 instance_count, Uint32 first_vertex, Uint32 first_instance) {
            funcs_.draw(backend_context_, vertex_count, instance_count, first_vertex, first_instance);
        }

        void draw_indexed(Uint32 index_count, Uint32 instance_count, Uint32 first_index, Int32 vertex_offset,
                          Uint32 first_instance) {
            funcs_.draw_indexed(backend_context_, index_count, instance_count, first_index, vertex_offset,
                                first_instance);
        }

        // ----------------------------------------------------------
        // Multi-draw indirect draw call.
        // ----------------------------------------------------------
        void multi_draw_indirect(Uint32 mode, Uint32 type, const void *indirect, Uint32 draw_count, Uint32 stride) {
            funcs_.multi_draw_indirect(backend_context_, mode, type, indirect, draw_count, stride);
        }

        void multi_draw_arrays_indirect(Uint32 mode, const void *indirect, Uint32 draw_count, Uint32 stride) {
            funcs_.multi_draw_arrays_indirect(backend_context_, mode, indirect, draw_count, stride);
        }

        // ----------------------------------------------------------
        // Buffer mapping functions.
        // ----------------------------------------------------------
        void *map_buffer(BufferHandle handle, Uint32 offset, Uint32 size, AccessFlags flags) {
            return funcs_.map_buffer(backend_context_, handle, offset, size, flags);
        }

        void unmap_buffer(BufferHandle handle) {
            funcs_.unmap_buffer(backend_context_, handle);
        }

        void update_buffer(BufferHandle handle, const void *data, Uint32 offset, Uint32 size) {
            funcs_.update_buffer(backend_context_, handle, data, offset, size);
        }

        // ---------------------------
        // Compute dispatch commands
        // ---------------------------
        void dispatch(Uint32 group_count_x, Uint32 group_count_y, Uint32 group_count_z) {
            funcs_.dispatch(backend_context_, group_count_x, group_count_y, group_count_z);
        }

        void dispatch_indirect(CommandBufferHandle cmd, BufferHandle indirect_buffer, Uint32 offset) {
            funcs_.dispatch_indirect(backend_context_, cmd, indirect_buffer, offset);
        }

        // ---------------------------
        // Pipeline binding and dynamic state commands
        // ---------------------------
        void bind_pipeline(CommandBufferHandle cmd, PipelineHandle pipeline) {
            funcs_.bind_pipeline(backend_context_, cmd, pipeline);
        }

        void
        bind_vertex_buffers(CommandBufferHandle cmd, Uint32 first_binding, const std::vector<BufferHandle> &buffers,
                            const std::vector<Uint32> &offsets) {
            funcs_.bind_vertex_buffers(backend_context_, cmd, first_binding, buffers.data(), offsets.data(),
                                       static_cast<Uint32>(buffers.size()));
        }

        void bind_index_buffer(CommandBufferHandle cmd, BufferHandle buffer, Uint32 offset, IndexType index_type) {
            funcs_.bind_index_buffer(backend_context_, cmd, buffer, offset, index_type);
        }

        void set_viewport(CommandBufferHandle cmd, const Viewport &viewport) {
            funcs_.set_viewport(backend_context_, cmd, viewport);
        }

        void set_scissor(CommandBufferHandle cmd, const Rect2D &rect) {
            funcs_.set_scissor(backend_context_, cmd, rect);
        }

        void
        update_push_constants(CommandBufferHandle cmd, PipelineLayoutHandle layout, ShaderStage stage, Uint32 offset,
                              Uint32 size, const void *data) {
            funcs_.update_push_constants(backend_context_, cmd, layout, stage, offset, size, data);
        }

        // ---------------------------
        // Resource copy operations
        // ---------------------------
        void copy_buffer(BufferHandle src, BufferHandle dst, Uint32 src_offset, Uint32 dst_offset, Uint32 size) {
            funcs_.copy_buffer(backend_context_, src, dst, src_offset, dst_offset, size);
        }

        void copy_texture(TextureHandle src, TextureHandle dst) {
            funcs_.copy_texture(backend_context_, src, dst);
        }

        // ---------------------------
        // Query and debug utilities
        // ---------------------------
        void reset_query_pool(QueryPoolHandle pool, Uint32 first_query, Uint32 query_count) {
            funcs_.reset_query_pool(backend_context_, pool, first_query, query_count);
        }

        void begin_query(CommandBufferHandle cmd, QueryPoolHandle pool, Uint32 query, const char *name) {
            funcs_.begin_query(backend_context_, cmd, pool, query, name);
        }

        void end_query(CommandBufferHandle cmd, QueryPoolHandle pool, Uint32 query) {
            funcs_.end_query(backend_context_, cmd, pool, query);
        }

        void
        get_query_results(QueryPoolHandle pool, Uint32 first_query, Uint32 query_count, void *results, Uint32 stride,
                          bool wait) {
            funcs_.get_query_results(backend_context_, pool, first_query, query_count, results, stride, wait);
        }

        void begin_debug_marker(CommandBufferHandle cmd, const char *marker_name, const float color[4]) {
            funcs_.begin_debug_marker(backend_context_, cmd, marker_name, color);
        }

        void end_debug_marker(CommandBufferHandle cmd) {
            funcs_.end_debug_marker(backend_context_, cmd);
        }

        void insert_debug_marker(CommandBufferHandle cmd, const char *marker_name, const float color[4]) {
            funcs_.insert_debug_marker(backend_context_, cmd, marker_name, color);
        }

        // ---------------------------
        // Pipeline layout creation/destruction
        // ---------------------------
        PipelineLayoutHandle create_pipeline_layout(const PipelineLayoutDesc &desc) {
            return funcs_.create_pipeline_layout(backend_context_, desc);
        }

        void destroy_pipeline_layout(PipelineLayoutHandle layout) {
            funcs_.destroy_pipeline_layout(backend_context_, layout);
        }

        // ---------------------------
        // Compute pipeline creation/destruction
        // ---------------------------
        PipelineHandle create_compute_pipeline(const ComputePipelineDesc &desc) {
            return funcs_.create_compute_pipeline(backend_context_, desc);
        }
        void destroy_compute_pipeline(PipelineHandle handle) {
            funcs_.destroy_compute_pipeline(backend_context_, handle);
        }

        // ---------------------------
        // Mesh pipeline creation/destruction
        // ---------------------------
        PipelineHandle create_mesh_pipeline(const MeshPipelineDesc &desc) {
            return funcs_.create_mesh_pipeline(backend_context_, desc);
        }
        void destroy_mesh_pipeline(PipelineHandle handle) {
            funcs_.destroy_mesh_pipeline(backend_context_, handle);
        }

        // Texture view creation/destruction
        TextureViewHandle create_texture_view(const TextureViewDesc &desc) {
            return funcs_.create_texture_view(backend_context_, desc);
        }
        void destroy_texture_view(TextureViewHandle handle) {
            funcs_.destroy_texture_view(backend_context_, handle);
        }

        // Render pass commands
        void begin_render_pass(CommandBufferHandle cmd, const RenderPassBeginInfo &begin_info) {
            funcs_.begin_render_pass(backend_context_, cmd, begin_info);
        }

        void end_render_pass(CommandBufferHandle cmd) {
            funcs_.end_render_pass(backend_context_, cmd);
        }

        [[nodiscard]] BackendType get_backend_type() const { return backend_type_; }

        [[nodiscard]] void *get_backend_context() const { return backend_context_; }

    protected:
        DeviceFunctions funcs_;
        void *backend_context_ = nullptr;
        BackendType backend_type_;
    };
} // namespace Bcg::Graphics
