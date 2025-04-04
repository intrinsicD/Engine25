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

    /**
     * @brief Describes the properties of a buffer to be created.
     *
     * This structure defines the size, memory usage, and usage type of a buffer.
     * It is used to specify the parameters for buffer creation in the graphics API.
     */
    struct BufferDesc {
        SizeType size = 0; ///< The size of the buffer in bytes.
        MemoryUsage memory_usage = MemoryUsage::GpuOnly; ///< The memory usage type of the buffer.
        BufferUsage usage = BufferUsage::VertexBuffer; ///< The intended usage of the buffer.
    };

    /**
     * @brief Describes the properties of a texture to be created.
     *
     * This structure defines the dimensions, format, type, usage, and filtering options
     * for a texture. It is used to specify the parameters for texture creation in the graphics API.
     */
    struct TextureDesc {
        Uint32 width = 0; ///< The width of the texture in pixels.
        Uint32 height = 0; ///< The height of the texture in pixels.
        Uint32 depth = 1; ///< The depth of the texture (for 3D textures).
        Uint32 mip_levels = 1; ///< The number of mipmap levels.
        Format format = Format::RGBA8; ///< The format of the texture.
        TextureType type = TextureType::Texture2D; ///< The type of the texture (e.g., 2D, 3D, CubeMap).
        TextureUsage usage = TextureUsage::Sampled; ///< The intended usage of the texture.
        Filter min_filter = Filter::Linear; ///< The minification filter to be used.
        Filter mag_filter = Filter::Linear; ///< The magnification filter to be used.
        AddressMode address_mode_u = AddressMode::Repeat; ///< The addressing mode for the U (horizontal) coordinate.
        AddressMode address_mode_v = AddressMode::Repeat; ///< The addressing mode for the V (vertical) coordinate.
        AddressMode address_mode_w = AddressMode::Repeat; ///< The addressing mode for the W (depth) coordinate.
    };
    /**
     * @brief Describes the properties of a sampler to be created.
     *
     * This structure defines the filtering, mipmap mode, addressing modes,
     * LOD bias, anisotropy, and border color for a sampler. It is used to
     * specify the parameters for sampler creation in the graphics API.
     */
    struct SamplerDesc {
        Filter min_filter = Filter::Linear; ///< The minification filter to be used.
        Filter mag_filter = Filter::Linear; ///< The magnification filter to be used.
        MipmapMode mipmap_mode = MipmapMode::Linear; ///< The mipmap mode to be used.
        AddressMode address_mode_u = AddressMode::Repeat; ///< The addressing mode for the U (horizontal) coordinate.
        AddressMode address_mode_v = AddressMode::Repeat; ///< The addressing mode for the V (vertical) coordinate.
        AddressMode address_mode_w = AddressMode::Repeat; ///< The addressing mode for the W (depth) coordinate.
        float mip_lod_bias = 0.0f; ///< The level of detail bias for mipmaps.
        float max_anisotropy = 1.0f; ///< The maximum anisotropy level.
        BorderColor border_color = BorderColor::OpaqueBlack; ///< The border color to be used if addressing mode is ClampToBorder.
    };

    /**
     * @brief Describes the properties of a shader to be created.
     *
     * This structure defines the stage, code, entry point, macros, and optimization
     * options for a shader. It is used to specify the parameters for shader creation
     * in the graphics API.
     */
    struct ShaderDesc {
        ShaderStage stage; ///< The stage of the shader (e.g., Vertex, Fragment).
        const void *code = nullptr; ///< Pointer to the shader code (SPIR-V, DXBC, DXIL, or backend-specific bytecode).
        SizeType code_size = 0; ///< The size of the shader code in bytes.
        const char *entry_point = "main"; ///< The entry point function name in the shader code.
        std::vector<std::string> macros; ///< A list of macros to be defined during shader compilation.
        bool optimize = true; ///< Flag indicating whether to optimize the shader code.
        bool debug_info = false; ///< Flag indicating whether to include debug information in the shader code.
    };

    /**
     * @brief Describes the properties of an input binding.
     *
     * This structure defines the binding index, stride, and input rate for a vertex input binding.
     * It is used to specify the parameters for vertex input bindings in the graphics pipeline.
     */
    struct InputBindingDesc {
        Uint32 binding = 0; ///< The binding index.
        Uint32 stride = 0; ///< The byte stride between consecutive elements.
        bool input_rate_instance = false; ///< true if per-instance data, false if per-vertex.
    };

    /**
     * @brief Describes the properties of an input attribute.
     *
     * This structure defines the location, format, offset, and binding for a vertex input attribute.
     * It is used to specify the parameters for vertex input attributes in the graphics pipeline.
     */
    struct InputAttributeDesc {
        Uint32 location = 0; ///< The location of the attribute in the shader.
        Format format = Format::RGBA8; ///< The format of the attribute data.
        Uint32 offset = 0; ///< The byte offset of the attribute data in the vertex.
        Uint32 binding = 0; ///< The binding index of the attribute.
    };

    /**
     * @brief Describes the layout of vertex input bindings and attributes.
     *
     * This structure defines the bindings and attributes for vertex input.
     * It is used to specify the parameters for vertex input layout in the graphics pipeline.
     */
    struct InputLayoutDesc {
        std::vector<InputBindingDesc> bindings; ///< A list of input bindings.
        std::vector<InputAttributeDesc> attributes; ///< A list of input attributes.
    };

    /**
     * @brief Describes the rasterization state for a graphics pipeline.
     *
     * This structure defines the rasterization state, including depth clamping,
     * fill mode, culling mode, front face winding, and depth bias settings.
     * It is used to specify the parameters for rasterization in the graphics pipeline.
     */
    struct RasterizationStateDesc {
        bool depth_clamp_enable = false; ///< Enable or disable depth clamping.
        PolygonMode fill_mode = PolygonMode::Fill; ///< The polygon fill mode (e.g., Fill, Line, Point).
        CullMode cull_mode = CullMode::Back; ///< The culling mode (e.g., None, Front, Back).
        FrontFace front_face = FrontFace::CounterClockwise; ///< The front face winding order (e.g., Clockwise, CounterClockwise).
        bool depth_bias_enable = false; ///< Enable or disable depth bias.
        float depth_bias_constant_factor = 0.0f; ///< The constant factor for depth bias.
        float depth_bias_clamp = 0.0f; ///< The maximum depth bias.
        float depth_bias_slope_factor = 0.0f; ///< The slope factor for depth bias.
    };

    /**
     * @brief Describes the stencil operations for a face.
     *
     * This structure defines the operations to be performed on the stencil buffer
     * for a face (front or back) during rendering.
     */
    struct StencilOpDesc {
        BlendOp fail_op = BlendOp::Add; ///< Operation to perform when stencil test fails.
        BlendOp pass_op = BlendOp::Add; ///< Operation to perform when stencil test passes.
        BlendOp depth_fail_op = BlendOp::Add; ///< Operation to perform when depth test fails.
        CompareOp compare_op = CompareOp::Always; ///< Comparison operation for the stencil test.
        Uint32 compare_mask = 0xFF; ///< Mask applied to the stencil value for comparison.
        Uint32 write_mask = 0xFF; ///< Mask applied to the stencil value for writing.
        Uint32 reference = 0; ///< Reference value for the stencil test.
    };

    /**
     * @brief Describes the depth and stencil state for a graphics pipeline.
     *
     * This structure defines the depth test, depth write, depth compare operation,
     * and stencil test settings. It is used to specify the parameters for depth
     * and stencil state in the graphics pipeline.
     */
    struct DepthStencilStateDesc {
        bool depth_test_enable = true; ///< Enable or disable depth testing.
        bool depth_write_enable = true; ///< Enable or disable writing to the depth buffer.
        CompareOp depth_compare_op = CompareOp::Less; ///< The comparison operation for the depth test.
        bool stencil_test_enable = false; ///< Enable or disable stencil testing.

        StencilOpDesc front; ///< Stencil operations for the front face.
        StencilOpDesc back; ///< Stencil operations for the back face.
    };

    /**
     * @brief Describes the blend state for a render target.
     *
     * This structure defines the blend state, including blend enable flag,
     * source and destination blend factors, blend operations for color and alpha,
     * and the color write mask. It is used to specify the parameters for blending
     * in the graphics pipeline.
     */
    struct RenderTargetBlendDesc {
        bool blend_enable = false; ///< Enable or disable blending.
        BlendFactor src_color_blend_factor = BlendFactor::One; ///< Source blend factor for color.
        BlendFactor dst_color_blend_factor = BlendFactor::Zero; ///< Destination blend factor for color.
        BlendOp color_blend_op = BlendOp::Add; ///< Blend operation for color.
        BlendFactor src_alpha_blend_factor = BlendFactor::One; ///< Source blend factor for alpha.
        BlendFactor dst_alpha_blend_factor = BlendFactor::Zero; ///< Destination blend factor for alpha.
        BlendOp alpha_blend_op = BlendOp::Add; ///< Blend operation for alpha.
        Uint32 color_write_mask = 0xF; ///< Color write mask (RGBA).
    };

    /**
     * @brief Describes the blend state for multiple render targets.
     *
     * This structure defines the blend state for a list of render targets.
     * It is used to specify the parameters for blending in the graphics pipeline.
     */
    struct BlendStateDesc {
        std::vector<RenderTargetBlendDesc> render_target_blends; ///< A list of blend states for render targets.
    };

    /**
     * @brief Describes a binding within a descriptor set layout.
     *
     * This structure defines the binding index, shader stage flags, and count
     * for a descriptor set layout binding. It is used to specify the parameters
     * for descriptor set layout bindings in the graphics pipeline.
     */
    struct DescriptorSetLayoutBinding {
        Uint32 binding = 0; ///< The binding index.
        ShaderStage stage_flags = ShaderStage::Vertex; ///< The shader stages that can access this binding.
        Uint32 count = 1; ///< The number of descriptors in the binding.
        // The descriptor type can be inferred at DescriptorSet creation,
        // or extended to be part of the layout if desired.
    };

    /**
     * @brief Describes the layout of a descriptor set.
     *
     * This structure defines the bindings for a descriptor set layout.
     * It is used to specify the parameters for descriptor set layout
     * in the graphics pipeline.
     */
    struct DescriptorSetLayoutDesc {
        std::vector<DescriptorSetLayoutBinding> bindings; ///< A list of descriptor set layout bindings.
    };

    /**
     * @brief Describes the layout of a pipeline.
     *
     * This structure defines the descriptor sets and push constants
     * for a pipeline layout. It is used to specify the parameters
     * for pipeline layout in the graphics pipeline.
     */
    struct PipelineLayoutDesc {
        std::vector<DescriptorSetLayoutDesc> descriptor_sets; ///< A list of descriptor set layouts.
        Uint32 push_constant_size = 0; ///< The size of the push constants in bytes.
        ShaderStage push_constant_stages = ShaderStage::Vertex; ///< The shader stages that can access the push constants.
    };

    /**
     * @brief Describes an attachment in a render pass.
     *
     * This structure defines the format, load and store operations,
     * depth/stencil flag, and sample count for an attachment.
     * It is used to specify the parameters for attachments in the graphics pipeline.
     */
    struct AttachmentDescription {
        Format format; ///< The format of the attachment.
        LoadOp load_op; ///< The load operation for the attachment.
        StoreOp store_op; ///< The store operation for the attachment.
        bool is_depth_stencil = false; ///< Flag indicating if the attachment is a depth/stencil buffer.
        SampleCount sample_count = SampleCount::Count1; ///< The number of samples for multisampling.
    };

    /**
      * @brief Describes a subpass in a render pass.
      *
      * This structure defines the color and depth/stencil attachments
      * for a subpass. It is used to specify the parameters for subpasses
      * in the graphics pipeline.
      */
    struct SubpassDescription {
        std::vector<Uint32> color_attachments; ///< A list of color attachment indices.
        Int32 depth_stencil_attachment = -1; ///< The index of the depth/stencil attachment, or -1 if not used.
    };

    /**
     * @brief Describes a render pass.
     *
     * This structure defines the attachments and subpasses for a render pass.
     * It is used to specify the parameters for render passes in the graphics pipeline.
     */
    struct RenderPassDesc {
        std::vector<AttachmentDescription> attachments; ///< A list of attachment descriptions.
        std::vector<SubpassDescription> subpasses; ///< A list of subpass descriptions.
    };

    /**
     * @brief Describes the properties of a graphics pipeline.
     *
     * This structure defines the shaders, input layout, rasterization state,
     * depth/stencil state, blend state, pipeline layout, and render pass
     * for a graphics pipeline. It is used to specify the parameters for
     * creating a graphics pipeline in the graphics API.
     */
    struct PipelineDesc {
        ShaderHandle vertex_shader; ///< Handle to the vertex shader.
        ShaderHandle fragment_shader; ///< Handle to the fragment shader.
        ShaderHandle geometry_shader = {}; ///< Handle to the geometry shader (optional).
        ShaderHandle tessellation_control_shader = {}; ///< Handle to the tessellation control shader (optional).
        ShaderHandle tessellation_evaluation_shader = {}; ///< Handle to the tessellation evaluation shader (optional).
        InputLayoutDesc input_layout; ///< Description of the vertex input layout.
        RasterizationStateDesc rasterization_state; ///< Description of the rasterization state.
        DepthStencilStateDesc depth_stencil_state; ///< Description of the depth/stencil state.
        BlendStateDesc blend_state; ///< Description of the blend state.
        PipelineLayoutDesc pipeline_layout; ///< Description of the pipeline layout.
        RenderPassHandle render_pass; ///< Handle to the render pass.
    };

    /**
     * @brief Describes the properties of a compute pipeline.
     *
     * This structure defines the compute shader and pipeline layout
     * for a compute pipeline. It is used to specify the parameters
     * for creating a compute pipeline in the graphics API.
     */
    struct ComputePipelineDesc {
        ShaderHandle compute_shader; ///< Handle to the compute shader.
        PipelineLayoutDesc pipeline_layout; ///< Description of the pipeline layout.
    };

    /**
     * @brief Describes the properties of a mesh pipeline.
     *
     * This structure defines the shaders, pipeline layout, rasterization state,
     * depth/stencil state, blend state, and render pass for a mesh pipeline.
     * It is used to specify the parameters for creating a mesh pipeline in the graphics API.
     */
    struct MeshPipelineDesc {
        ShaderHandle mesh_shader; ///< Handle to the mesh shader.
        ShaderHandle fragment_shader; ///< Handle to the fragment shader.
        PipelineLayoutDesc pipeline_layout; ///< Description of the pipeline layout.
        RasterizationStateDesc rasterization_state; ///< Description of the rasterization state.
        DepthStencilStateDesc depth_stencil_state; ///< Description of the depth/stencil state.
        BlendStateDesc blend_state; ///< Description of the blend state.
        RenderPassHandle render_pass; ///< Handle to the render pass.
    };

    /**
     * @brief Describes the properties of a texture view.
     *
     * This structure defines the texture, format, mip levels, and array layers
     * for a texture view. It is used to specify the parameters for creating
     * a texture view in the graphics API.
     */
    struct TextureViewDesc {
        TextureHandle texture; ///< Handle to the texture.
        Format format; ///< Format of the texture view. Allows reinterpretation if needed.
        Uint32 base_mip_level = 0; ///< The first mip level accessible to the view.
        Uint32 level_count = 1; ///< The number of mip levels accessible to the view.
        Uint32 base_array_layer = 0; ///< The first array layer accessible to the view.
        Uint32 layer_count = 1; ///< The number of array layers accessible to the view.
    };

    /**
     * @brief Describes the properties of a command buffer.
     *
     * This structure defines whether the command buffer is primary,
     * can be used for graphics or compute operations, and if it allows
     * simultaneous use. It is used to specify the parameters for
     * creating a command buffer in the graphics API.
     */
    struct CommandBufferDesc {
        bool is_primary = true; ///< Indicates if the command buffer is primary.
        bool is_graphics = true; ///< Indicates if the command buffer can be used for graphics operations.
        bool is_compute = false; ///< Indicates if the command buffer can be used for compute operations.
        bool allow_simultaneous_use = false; ///< Indicates if the command buffer allows simultaneous use.
    };

    /**
     * @brief Describes the properties of a swapchain.
     *
     * This structure defines the width, height, format, image count,
     * and present mode for a swapchain. It is used to specify the parameters
     * for creating a swapchain in the graphics API.
     */
    struct SwapchainDesc {
        Uint32 width; ///< The width of the swapchain in pixels.
        Uint32 height; ///< The height of the swapchain in pixels.
        Format format; ///< The format of the swapchain images.
        Uint32 image_count; ///< The number of images in the swapchain.
        PresentMode present_mode; ///< The present mode of the swapchain.
    };

    /**
      * @brief Describes a binding within a descriptor set.
      *
      * This structure defines the binding index, descriptor type, buffer handle,
      * buffer offset, buffer range, texture handle, and sampler handle for a descriptor set.
      * It is used to specify the parameters for descriptor bindings in the graphics pipeline.
      */
    struct DescriptorBinding {
        Uint32 binding = 0; ///< The binding index.
        DescriptorType type = DescriptorType::UniformBuffer; ///< The type of the descriptor.
        BufferHandle buffer = {}; ///< Handle to the buffer.
        SizeType buffer_offset = 0; ///< Offset within the buffer.
        SizeType buffer_range = 0; ///< Range within the buffer.
        TextureHandle texture = {}; ///< Handle to the texture.
        SamplerHandle sampler = {}; ///< Handle to the sampler.
    };

    /**
     * @brief Describes an attachment in a framebuffer.
     *
     * This structure defines the texture, mip level, base layer, and layer count
     * for a framebuffer attachment. It is used to specify the parameters for
     * framebuffer attachments in the graphics API.
     */
    struct FramebufferAttachmentDesc {
        TextureHandle texture; ///< Handle to the texture.
        Uint32 mip_level = 0; ///< The mip level of the texture to be used.
        Uint32 base_layer = 0; ///< The first layer of the texture to be used.
        Uint32 layer_count = 1; ///< The number of layers of the texture to be used.
    };

    /**
     * @brief Describes the properties of a framebuffer.
     *
     * This structure defines the render pass, attachments, width, height,
     * and layer count for a framebuffer. It is used to specify the parameters
     * for creating a framebuffer in the graphics API.
     */
    struct FramebufferDesc {
        RenderPassHandle render_pass; ///< Handle to the render pass.
        std::vector<FramebufferAttachmentDesc> attachments; ///< A list of framebuffer attachments.
        Uint32 width = 0; ///< The width of the framebuffer in pixels.
        Uint32 height = 0; ///< The height of the framebuffer in pixels.
        Uint32 layers = 1; ///< The number of layers in the framebuffer.
    };

    /**
     * @brief Describes the properties of a descriptor set.
     *
     * This structure defines the layout and bindings for a descriptor set.
     * It is used to specify the parameters for creating a descriptor set
     * in the graphics API.
     */
    struct DescriptorSetDesc {
        DescriptorSetLayoutHandle layout; ///< Handle to the descriptor set layout.
        std::vector<DescriptorBinding> bindings; ///< A list of descriptor bindings.
    };

    /**
     * @brief Describes the viewport state.
     *
     * This structure defines the x and y coordinates, width, height,
     * minimum depth, and maximum depth for a viewport. It is used to
     * specify the parameters for viewport state in the graphics pipeline.
     */
    struct Viewport {
        float x = 0.0f; ///< The x coordinate of the viewport.
        float y = 0.0f; ///< The y coordinate of the viewport.
        float width = 0.0f; ///< The width of the viewport.
        float height = 0.0f; ///< The height of the viewport.
        float min_depth = 0.0f; ///< The minimum depth of the viewport.
        float max_depth = 1.0f; ///< The maximum depth of the viewport.
    };

    /**
     * @brief Describes a 2D rectangle.
     *
     * This structure defines the x and y coordinates, width, and height
     * of a rectangle. It is used for specifying scissor state in the graphics pipeline.
     */
    struct Rect2D {
        Int32 x = 0; ///< The x coordinate of the rectangle.
        Int32 y = 0; ///< The y coordinate of the rectangle.
        Uint32 width = 0; ///< The width of the rectangle.
        Uint32 height = 0; ///< The height of the rectangle.
    };


    /**
     * @brief Represents a clear value for a framebuffer attachment.
     *
     * This structure defines a union that can hold either a color value
     * (with red, green, blue, and alpha components) or a depth-stencil value
     * (with depth and stencil components). It is used to specify the clear
     * values for framebuffer attachments in the graphics API.
     */
    struct ClearValue {
        union {
            struct {
                float r, g, b, a; ///< Color components: red, green, blue, and alpha.
            } color;
            struct {
                float depth; ///< Depth component.
                Uint32 stencil; ///< Stencil component.
            } depth_stencil;
        };
    };

    /**
     * @brief Describes the information required to begin a render pass.
     *
     * This structure defines the render pass handle, target framebuffer,
     * render area, and clear values for attachments. It is used to specify
     * the parameters for beginning a render pass in the graphics API.
     */
    struct RenderPassBeginInfo {
        RenderPassHandle render_pass;          ///< Render pass handle to use.
        FramebufferHandle framebuffer;         ///< Target framebuffer.
        Rect2D render_area;                    ///< Render area (x, y, width, height).
        std::vector<ClearValue> clear_values;  ///< Clear values for attachments.
    };

    /**
     * @brief Handle for pipeline layouts.
     *
     * This structure represents a handle for pipeline layouts, which are used with push constants.
     */
    struct PipelineLayoutHandle {
        Uint32 id = 0; ///< Identifier for the pipeline layout.
    };

    /**
     * @brief Handle for query pools.
     *
     * This structure represents a handle for query pools, which are used to manage
     * a collection of queries in the graphics API.
     */
    struct QueryPoolHandle {
        Uint32 id = 0; ///< Identifier for the query pool.
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

    /**
     * @brief Describes the information required to submit command buffers to a queue.
     *
     * This structure defines the command buffers to be executed, semaphores to wait on before execution,
     * semaphores to signal after execution, and an optional fence to signal when done.
     */
    struct SubmitInfo {
        std::vector<CommandBufferHandle> command_buffers; ///< Command buffers to be executed.
        std::vector<SemaphoreHandle> wait_semaphores; ///< Semaphores to wait on before execution.
        std::vector<SemaphoreHandle> signal_semaphores; ///< Semaphores to signal after execution.
        FenceHandle fence = {}; ///< Optional fence to signal when done.
    };

    /**
     * @brief Describes the information required to present an image to a swapchain.
     *
     * This structure defines the swapchain handle, image index, and semaphores to wait on before presentation.
     */
    struct PresentInfo {
        SwapchainHandle swapchain; ///< Handle to the swapchain.
        Uint32 image_index; ///< Index of the image to present.
        std::vector<SemaphoreHandle> wait_semaphores; ///< Semaphores to wait on before presentation.
    };

    /**
     * @brief Update info for descriptors after creation.
     *
     * This structure defines the descriptor set handle and binding information
     * for updating descriptors after they have been created.
     */
    struct DescriptorUpdate {
        DescriptorSetHandle descriptor_set; ///< Handle to the descriptor set.
        DescriptorBinding binding_info; ///< Information about the descriptor binding.
    };

    // ----------------------------------------------------------
    // Draw*IndirectCommand Structure
    // ----------------------------------------------------------
    /**
     * @brief Structure representing a draw command for indexed drawing.
     *
     * This structure defines the parameters for an indirect draw command
     * that uses an index buffer. It is used to specify the number of indices,
     * instances, and offsets for drawing.
     */
    struct DrawElementsIndirectCommand {
        Uint32 count; ///< Number of indices per draw.
        Uint32 instance_count; ///< Number of instances to draw.
        Uint32 first_index; ///< Offset in the index buffer.
        Uint32 base_vertex; ///< Added to each index.
        Uint32 base_instance; ///< Starting instance ID.
    };

    /**
     * @brief Structure representing a draw command for non-indexed drawing.
     *
     * This structure defines the parameters for an indirect draw command
     * that does not use an index buffer. It is used to specify the number
     * of vertices, instances, and offsets for drawing.
     */
    struct DrawArraysIndirectCommand {
        Uint32 count; ///< Number of vertices to draw per command.
        Uint32 instance_count; ///< Number of instances to draw.
        Uint32 first; ///< Starting vertex offset.
        Uint32 base_instance; ///< Starting instance ID.
    };

    // ----------------------------------------------------------
    // Device Functions Table
    //
    // This structure holds function pointers for all operations.
    // A backend implementation sets these pointers to functions that
    // carry out the actual work. The Device class then calls them.
    // ----------------------------------------------------------
    /**
     * @brief Structure holding function pointers for device operations.
     *
     * This structure contains function pointers for various device operations,
     * including resource creation/destruction, command buffer recording,
     * synchronization, and rendering commands. These functions are implemented
     * by the backend and called by the Device class.
     */
    struct DeviceFunctions {
        // Resource creation/destruction
        BufferHandle (*create_buffer)(void *ctx, const BufferDesc &) = nullptr; ///< Function to create a buffer.
        void (*destroy_buffer)(void *ctx, BufferHandle) = nullptr; ///< Function to destroy a buffer.

        TextureHandle (*create_texture)(void *ctx, const TextureDesc &) = nullptr; ///< Function to create a texture.
        void (*destroy_texture)(void *ctx, TextureHandle) = nullptr; ///< Function to destroy a texture.

        SamplerHandle (*create_sampler)(void *ctx, const SamplerDesc &) = nullptr; ///< Function to create a sampler.
        void (*destroy_sampler)(void *ctx, SamplerHandle) = nullptr; ///< Function to destroy a sampler.

        ShaderHandle (*create_shader)(void *ctx, const ShaderDesc &) = nullptr; ///< Function to create a shader.
        void (*destroy_shader)(void *ctx, ShaderHandle) = nullptr; ///< Function to destroy a shader.

        PipelineHandle
        (*create_pipeline)(void *ctx, const PipelineDesc &) = nullptr; ///< Function to create a pipeline.
        void (*destroy_pipeline)(void *ctx, PipelineHandle) = nullptr; ///< Function to destroy a pipeline.

        RenderPassHandle
        (*create_render_pass)(void *ctx, const RenderPassDesc &) = nullptr; ///< Function to create a render pass.
        void (*destroy_render_pass)(void *ctx, RenderPassHandle) = nullptr; ///< Function to destroy a render pass.

        FramebufferHandle
        (*create_framebuffer)(void *ctx, const FramebufferDesc &) = nullptr; ///< Function to create a framebuffer.
        void (*destroy_framebuffer)(void *ctx, FramebufferHandle) = nullptr; ///< Function to destroy a framebuffer.

        DescriptorSetLayoutHandle (*create_descriptor_set_layout)(void *ctx,
                                                                  const DescriptorSetLayoutDesc &) = nullptr; ///< Function to create a descriptor set layout.
        void (*destroy_descriptor_set_layout)(void *ctx,
                                              DescriptorSetLayoutHandle) = nullptr; ///< Function to destroy a descriptor set layout.

        DescriptorSetHandle (*create_descriptor_set)(void *ctx,
                                                     const DescriptorSetDesc &) = nullptr; ///< Function to create a descriptor set.
        void
        (*destroy_descriptor_set)(void *ctx, DescriptorSetHandle) = nullptr; ///< Function to destroy a descriptor set.

        CommandBufferHandle (*create_command_buffer)(void *ctx,
                                                     const CommandBufferDesc &) = nullptr; ///< Function to create a command buffer.
        void
        (*destroy_command_buffer)(void *ctx, CommandBufferHandle) = nullptr; ///< Function to destroy a command buffer.

        FenceHandle (*create_fence)(void *ctx) = nullptr; ///< Function to create a fence.
        void (*destroy_fence)(void *ctx, FenceHandle) = nullptr; ///< Function to destroy a fence.

        SemaphoreHandle (*create_semaphore)(void *ctx) = nullptr; ///< Function to create a semaphore.
        void (*destroy_semaphore)(void *ctx, SemaphoreHandle) = nullptr; ///< Function to destroy a semaphore.

        SwapchainHandle
        (*create_swapchain)(void *ctx, const SwapchainDesc &) = nullptr; ///< Function to create a swapchain.
        void (*destroy_swapchain)(void *ctx, SwapchainHandle) = nullptr; ///< Function to destroy a swapchain.

        // Memory barriers for resource synchronization
        void (*device_barrier)(void *ctx, PipelineStage src_stage, PipelineStage dst_stage,
                               AccessFlags src_access,
                               AccessFlags dst_access) = nullptr; ///< Function to set a device barrier.

        bool (*is_feature_supported)(void *ctx,
                                     DeviceFeature feature) = nullptr; ///< Function to check if a feature is supported.

        DeviceErrorType (*get_last_error_type)(void *ctx) = nullptr; ///< Function to get the last error type.
        void (*clear_last_error)(void *ctx) = nullptr; ///< Function to clear the last error.

        // Command buffer recording
        void (*begin_command_buffer)(void *ctx,
                                     CommandBufferHandle) = nullptr; ///< Function to begin recording a command buffer.
        void (*end_command_buffer)(void *ctx,
                                   CommandBufferHandle) = nullptr; ///< Function to end recording a command buffer.

        // Submitting command buffers
        void (*submit)(void *ctx, const SubmitInfo &) = nullptr; ///< Function to submit command buffers to a queue.
        void (*wait_for_fences)(void *ctx, const std::vector<FenceHandle> &fences, bool wait_all,
                                Uint64 timeout) = nullptr; ///< Function to wait for fences.
        void
        (*reset_fences)(void *ctx, const std::vector<FenceHandle> &fences) = nullptr; ///< Function to reset fences.

        // Swapchain operations
        bool
        (*acquire_next_image)(void *ctx, SwapchainHandle, Uint64 timeout, SemaphoreHandle semaphore, FenceHandle fence,
                              Uint32 *image_index) = nullptr; ///< Function to acquire the next image from the swapchain.
        void (*present)(void *ctx, const PresentInfo &) = nullptr; ///< Function to present an image to the swapchain.

        // Descriptor set updates
        void (*update_descriptor_sets)(void *ctx,
                                       const std::vector<DescriptorUpdate> &) = nullptr; ///< Function to update descriptor sets.

        // Direct draw commands
        void (*draw)(void *ctx, Uint32 vertex_count, Uint32 instance_count, Uint32 first_vertex,
                     Uint32 first_instance) = nullptr; ///< Function to issue a draw command.
        void
        (*draw_indexed)(void *ctx, Uint32 index_count, Uint32 instance_count, Uint32 first_index, Int32 vertex_offset,
                        Uint32 first_instance) = nullptr; ///< Function to issue a draw command with an index buffer.

        // Multi-draw indirect draw call support
        void (*multi_draw_indirect)(void *ctx, Uint32 mode, Uint32 type, const void *indirect, Uint32 draw_count,
                                    Uint32 stride) = nullptr; ///< Function to issue multiple indirect draw commands.
        void (*multi_draw_arrays_indirect)(void *ctx, Uint32 mode, const void *indirect, Uint32 draw_count,
                                           Uint32 stride) = nullptr; ///< Function to issue multiple indirect draw commands for non-indexed drawing.

        // Buffer mapping functions
        void *(*map_buffer)(void *ctx, BufferHandle, Uint32 offset, Uint32 size,
                            AccessFlags flags) = nullptr; ///< Function to map a buffer.
        void (*unmap_buffer)(void *ctx, BufferHandle) = nullptr; ///< Function to unmap a buffer.
        void (*update_buffer)(void *ctx, BufferHandle, const void *data, Uint32 offset,
                              Uint32 size) = nullptr; ///< Function to update a buffer.

        // Compute dispatch commands
        void (*dispatch)(void *ctx, Uint32 group_count_x, Uint32 group_count_y,
                         Uint32 group_count_z) = nullptr; ///< Function to dispatch a compute command.
        void (*dispatch_indirect)(void *ctx, CommandBufferHandle cmd, BufferHandle indirect_buffer,
                                  Uint32 offset) = nullptr; ///< Function to dispatch an indirect compute command.

        // Pipeline binding and dynamic state commands
        void (*bind_pipeline)(void *ctx, CommandBufferHandle cmd,
                              PipelineHandle pipeline) = nullptr; ///< Function to bind a pipeline.
        void
        (*bind_vertex_buffers)(void *ctx, CommandBufferHandle cmd, Uint32 first_binding, const BufferHandle *buffers,
                               const Uint32 *offsets, Uint32 count) = nullptr; ///< Function to bind vertex buffers.
        void (*bind_index_buffer)(void *ctx, CommandBufferHandle cmd, BufferHandle buffer, Uint32 offset,
                                  IndexType index_type) = nullptr; ///< Function to bind an index buffer.
        void (*set_viewport)(void *ctx, CommandBufferHandle cmd,
                             const Viewport &viewport) = nullptr; ///< Function to set the viewport.
        void (*set_scissor)(void *ctx, CommandBufferHandle cmd,
                            const Rect2D &rect) = nullptr; ///< Function to set the scissor rectangle.
        void
        (*update_push_constants)(void *ctx, CommandBufferHandle cmd, PipelineLayoutHandle layout, ShaderStage stage,
                                 Uint32 offset, Uint32 size,
                                 const void *data) = nullptr; ///< Function to update push constants.

        // Resource copy operations
        void (*copy_buffer)(void *ctx, BufferHandle src, BufferHandle dst, Uint32 src_offset, Uint32 dst_offset,
                            Uint32 size) = nullptr; ///< Function to copy data between buffers.
        void (*copy_texture)(void *ctx, TextureHandle src,
                             TextureHandle dst) = nullptr; ///< Function to copy data between textures.

        // Query and debug utilities
        void (*reset_query_pool)(void *ctx, QueryPoolHandle pool, Uint32 first_query,
                                 Uint32 query_count) = nullptr; ///< Function to reset a query pool.
        void (*begin_query)(void *ctx, CommandBufferHandle cmd, QueryPoolHandle pool, Uint32 query,
                            const char *name) = nullptr; ///< Function to begin a query.
        void (*end_query)(void *ctx, CommandBufferHandle cmd, QueryPoolHandle pool,
                          Uint32 query) = nullptr; ///< Function to end a query.
        void
        (*get_query_results)(void *ctx, QueryPoolHandle pool, Uint32 first_query, Uint32 query_count, void *results,
                             Uint32 stride, bool wait) = nullptr; ///< Function to get query results.
        void (*begin_debug_marker)(void *ctx, CommandBufferHandle cmd, const char *marker_name,
                                   const float color[4]) = nullptr; ///< Function to begin a debug marker.
        void (*end_debug_marker)(void *ctx, CommandBufferHandle cmd) = nullptr; ///< Function to end a debug marker.
        void (*insert_debug_marker)(void *ctx, CommandBufferHandle cmd, const char *marker_name,
                                    const float color[4]) = nullptr; ///< Function to insert a debug marker.

        // Pipeline layout creation/destruction
        PipelineLayoutHandle (*create_pipeline_layout)(void *ctx,
                                                       const PipelineLayoutDesc &desc) = nullptr; ///< Function to create a pipeline layout.
        void (*destroy_pipeline_layout)(void *ctx,
                                        PipelineLayoutHandle layout) = nullptr; ///< Function to destroy a pipeline layout.

        // Compute Pipeline creation/destruction
        PipelineHandle (*create_compute_pipeline)(void *ctx,
                                                  const ComputePipelineDesc &) = nullptr; ///< Function to create a compute pipeline.
        void
        (*destroy_compute_pipeline)(void *ctx, PipelineHandle) = nullptr; ///< Function to destroy a compute pipeline.

        // Mesh Pipeline creation/destruction
        PipelineHandle
        (*create_mesh_pipeline)(void *ctx, const MeshPipelineDesc &) = nullptr; ///< Function to create a mesh pipeline.
        void (*destroy_mesh_pipeline)(void *ctx, PipelineHandle) = nullptr; ///< Function to destroy a mesh pipeline.

        // Texture view creation/destruction
        TextureViewHandle
        (*create_texture_view)(void *ctx, const TextureViewDesc &) = nullptr; ///< Function to create a texture view.
        void (*destroy_texture_view)(void *ctx, TextureViewHandle) = nullptr; ///< Function to destroy a texture view.

        // Render pass commands
        void (*begin_render_pass)(void *ctx, CommandBufferHandle cmd,
                                  const RenderPassBeginInfo &begin_info) = nullptr; ///< Function to begin a render pass.
        void (*end_render_pass)(void *ctx, CommandBufferHandle cmd) = nullptr; ///< Function to end a render pass.
    };

    // ----------------------------------------------------------
    // Device Class
    //
    // This class provides a C++ interface over the raw function pointers.
    // Users of the API typically create a Device instance with a fully
    // populated DeviceFunctions and a backend context, then use these
    // methods to create and manage resources.
    // ----------------------------------------------------------
    /**
             * @brief Class representing a device for managing GPU resources and operations.
             *
             * This class provides a C++ interface over the raw function pointers for various
             * device operations, including resource creation/destruction, command buffer recording,
             * synchronization, and rendering commands. The Device class is initialized with a
             * DeviceFunctions structure and a backend context.
             */
    class Device {
    public:
        /**
         * @brief Constructor for the Device class.
         *
         * @param funcs The structure containing function pointers for device operations.
         * @param backend_context The context for the backend implementation.
         * @param backend_type The type of the backend.
         */
        Device(const DeviceFunctions &funcs, void *backend_context, BackendType backend_type)
                : funcs_(funcs), backend_context_(backend_context), backend_type_(backend_type) {
        }

        // Resource Creation/Destruction

        /**
         * @brief Creates a buffer.
         *
         * @param desc The description of the buffer to create.
         * @return The handle to the created buffer.
         */
        BufferHandle create_buffer(const BufferDesc &desc) { return funcs_.create_buffer(backend_context_, desc); }

        /**
         * @brief Destroys a buffer.
         *
         * @param handle The handle to the buffer to destroy.
         */
        void destroy_buffer(BufferHandle handle) { funcs_.destroy_buffer(backend_context_, handle); }

        /**
         * @brief Creates a texture.
         *
         * @param desc The description of the texture to create.
         * @return The handle to the created texture.
         */
        TextureHandle create_texture(const TextureDesc &desc) { return funcs_.create_texture(backend_context_, desc); }

        /**
         * @brief Destroys a texture.
         *
         * @param handle The handle to the texture to destroy.
         */
        void destroy_texture(TextureHandle handle) { funcs_.destroy_texture(backend_context_, handle); }

        /**
         * @brief Creates a sampler.
         *
         * @param desc The description of the sampler to create.
         * @return The handle to the created sampler.
         */
        SamplerHandle create_sampler(const SamplerDesc &desc) { return funcs_.create_sampler(backend_context_, desc); }

        /**
         * @brief Destroys a sampler.
         *
         * @param handle The handle to the sampler to destroy.
         */
        void destroy_sampler(SamplerHandle handle) { funcs_.destroy_sampler(backend_context_, handle); }

        /**
         * @brief Creates a shader.
         *
         * @param desc The description of the shader to create.
         * @return The handle to the created shader.
         */
        ShaderHandle create_shader(const ShaderDesc &desc) { return funcs_.create_shader(backend_context_, desc); }

        /**
         * @brief Destroys a shader.
         *
         * @param handle The handle to the shader to destroy.
         */
        void destroy_shader(ShaderHandle handle) { funcs_.destroy_shader(backend_context_, handle); }

        /**
         * @brief Creates a pipeline.
         *
         * @param desc The description of the pipeline to create.
         * @return The handle to the created pipeline.
         */
        PipelineHandle create_pipeline(const PipelineDesc &desc) {
            return funcs_.create_pipeline(backend_context_, desc);
        }

        /**
         * @brief Destroys a pipeline.
         *
         * @param handle The handle to the pipeline to destroy.
         */
        void destroy_pipeline(PipelineHandle handle) { funcs_.destroy_pipeline(backend_context_, handle); }

        /**
         * @brief Creates a render pass.
         *
         * @param desc The description of the render pass to create.
         * @return The handle to the created render pass.
         */
        RenderPassHandle create_render_pass(const RenderPassDesc &desc) {
            return funcs_.create_render_pass(backend_context_, desc);
        }

        /**
         * @brief Destroys a render pass.
         *
         * @param handle The handle to the render pass to destroy.
         */
        void destroy_render_pass(RenderPassHandle handle) { funcs_.destroy_render_pass(backend_context_, handle); }

        /**
         * @brief Creates a framebuffer.
         *
         * @param desc The description of the framebuffer to create.
         * @return The handle to the created framebuffer.
         */
        FramebufferHandle create_framebuffer(const FramebufferDesc &desc) {
            return funcs_.create_framebuffer(backend_context_, desc);
        }

        /**
         * @brief Destroys a framebuffer.
         *
         * @param handle The handle to the framebuffer to destroy.
         */
        void destroy_framebuffer(FramebufferHandle handle) { funcs_.destroy_framebuffer(backend_context_, handle); }

        /**
         * @brief Creates a descriptor set layout.
         *
         * @param desc The description of the descriptor set layout to create.
         * @return The handle to the created descriptor set layout.
         */
        DescriptorSetLayoutHandle create_descriptor_set_layout(const DescriptorSetLayoutDesc &desc) {
            return funcs_.create_descriptor_set_layout(backend_context_, desc);
        }

        /**
         * @brief Destroys a descriptor set layout.
         *
         * @param handle The handle to the descriptor set layout to destroy.
         */
        void destroy_descriptor_set_layout(DescriptorSetLayoutHandle handle) {
            funcs_.destroy_descriptor_set_layout(backend_context_, handle);
        }

        /**
         * @brief Creates a descriptor set.
         *
         * @param desc The description of the descriptor set to create.
         * @return The handle to the created descriptor set.
         */
        DescriptorSetHandle create_descriptor_set(const DescriptorSetDesc &desc) {
            return funcs_.create_descriptor_set(backend_context_, desc);
        }

        /**
         * @brief Destroys a descriptor set.
         *
         * @param handle The handle to the descriptor set to destroy.
         */
        void destroy_descriptor_set(DescriptorSetHandle handle) {
            funcs_.destroy_descriptor_set(backend_context_, handle);
        }

        /**
         * @brief Creates a command buffer.
         *
         * @param desc The description of the command buffer to create.
         * @return The handle to the created command buffer.
         */
        CommandBufferHandle create_command_buffer(const CommandBufferDesc &desc) {
            return funcs_.create_command_buffer(backend_context_, desc);
        }

        /**
         * @brief Destroys a command buffer.
         *
         * @param handle The handle to the command buffer to destroy.
         */
        void destroy_command_buffer(CommandBufferHandle handle) {
            funcs_.destroy_command_buffer(backend_context_, handle);
        }

        /**
         * @brief Creates a fence.
         *
         * @return The handle to the created fence.
         */
        FenceHandle create_fence() { return funcs_.create_fence(backend_context_); }

        /**
         * @brief Destroys a fence.
         *
         * @param handle The handle to the fence to destroy.
         */
        void destroy_fence(FenceHandle handle) { funcs_.destroy_fence(backend_context_, handle); }

        /**
         * @brief Creates a semaphore.
         *
         * @return The handle to the created semaphore.
         */
        SemaphoreHandle create_semaphore() { return funcs_.create_semaphore(backend_context_); }

        /**
         * @brief Destroys a semaphore.
         *
         * @param handle The handle to the semaphore to destroy.
         */
        void destroy_semaphore(SemaphoreHandle handle) { funcs_.destroy_semaphore(backend_context_, handle); }

        /**
         * @brief Creates a swapchain.
         *
         * @param desc The description of the swapchain to create.
         * @return The handle to the created swapchain.
         */
        SwapchainHandle create_swapchain(const SwapchainDesc &desc) {
            return funcs_.create_swapchain(backend_context_, desc);
        }

        /**
         * @brief Destroys a swapchain.
         *
         * @param handle The handle to the swapchain to destroy.
         */
        void destroy_swapchain(SwapchainHandle handle) { funcs_.destroy_swapchain(backend_context_, handle); }

        /**
         * @brief Sets a device barrier for resource synchronization.
         *
         * @param src_stage The source pipeline stage.
         * @param dst_stage The destination pipeline stage.
         * @param src_access The source access flags.
         * @param dst_access The destination access flags.
         */
        void device_barrier(PipelineStage src_stage, PipelineStage dst_stage, AccessFlags src_access,
                            AccessFlags dst_access) {
            funcs_.device_barrier(backend_context_, src_stage, dst_stage, src_access, dst_access);
        }

        /**
         * @brief Checks if a feature is supported by the device.
         *
         * @param feature The feature to check.
         * @return True if the feature is supported, false otherwise.
         */
        [[nodiscard]] bool is_feature_supported(DeviceFeature feature) const {
            return funcs_.is_feature_supported(backend_context_, feature);
        }

        /**
         * @brief Gets the last error type encountered by the device.
         *
         * @return The last error type.
         */
        [[nodiscard]] DeviceErrorType get_last_error_type() const {
            return funcs_.get_last_error_type(backend_context_);
        }

        /**
         * @brief Clears the last error encountered by the device.
         */
        void clear_last_error() { funcs_.clear_last_error(backend_context_); }

        // Command buffer recording

        /**
         * @brief Begins recording a command buffer.
         *
         * @param cmd The handle to the command buffer to begin recording.
         */
        void begin_command_buffer(CommandBufferHandle cmd) { funcs_.begin_command_buffer(backend_context_, cmd); }

        /**
         * @brief Ends recording a command buffer.
         *
         * @param cmd The handle to the command buffer to end recording.
         */
        void end_command_buffer(CommandBufferHandle cmd) { funcs_.end_command_buffer(backend_context_, cmd); }

        // Submitting command buffers

        /**
         * @brief Submits command buffers to a queue.
         *
         * @param info The information required to submit the command buffers.
         */
        void submit(const SubmitInfo &info) { funcs_.submit(backend_context_, info); }

        /**
         * @brief Waits for fences to be signaled.
         *
         * @param fences The list of fences to wait for.
         * @param wait_all Whether to wait for all fences or any fence.
         * @param timeout The timeout period in nanoseconds.
         */
        void wait_for_fences(const std::vector<FenceHandle> &fences, bool wait_all, Uint64 timeout) {
            funcs_.wait_for_fences(backend_context_, fences, wait_all, timeout);
        }

        /**
         * @brief Resets fences to the unsignaled state.
         *
         * @param fences The list of fences to reset.
         */
        void reset_fences(const std::vector<FenceHandle> &fences) { funcs_.reset_fences(backend_context_, fences); }

        // Swapchain operations

        /**
         * @brief Acquires the next image from the swapchain.
         *
         * @param swapchain The handle to the swapchain.
         * @param timeout The timeout period in nanoseconds.
         * @param semaphore The semaphore to signal when the image is available.
         * @param fence The fence to signal when the image is available.
         * @param image_index The index of the acquired image.
         * @return True if the image was acquired successfully, false otherwise.
         */
        bool acquire_next_image(SwapchainHandle swapchain, Uint64 timeout, SemaphoreHandle semaphore, FenceHandle fence,
                                Uint32 *image_index) {
            return funcs_.acquire_next_image(backend_context_, swapchain, timeout, semaphore, fence, image_index);
        }

        /**
         * @brief Presents an image to the swapchain.
         *
         * @param info The information required to present the image.
         */
        void present(const PresentInfo &info) { funcs_.present(backend_context_, info); }

        // Descriptor set updates

        /**
         * @brief Updates descriptor sets.
         *
         * @param updates The list of descriptor updates.
         */
        void update_descriptor_sets(const std::vector<DescriptorUpdate> &updates) {
            funcs_.update_descriptor_sets(backend_context_, updates);
        }

        // Direct draw commands

        /**
         * @brief Issues a draw command.
         *
         * @param vertex_count The number of vertices to draw.
         * @param instance_count The number of instances to draw.
         * @param first_vertex The starting vertex offset.
         * @param first_instance The starting instance ID.
         */
        void draw(Uint32 vertex_count, Uint32 instance_count, Uint32 first_vertex, Uint32 first_instance) {
            funcs_.draw(backend_context_, vertex_count, instance_count, first_vertex, first_instance);
        }

        /**
         * @brief Issues a draw command with an index buffer.
         *
         * @param index_count The number of indices to draw.
         * @param instance_count The number of instances to draw.
         * @param first_index The starting index offset.
         * @param vertex_offset The vertex offset added to each index.
         * @param first_instance The starting instance ID.
         */
        void draw_indexed(Uint32 index_count, Uint32 instance_count, Uint32 first_index, Int32 vertex_offset,
                          Uint32 first_instance) {
            funcs_.draw_indexed(backend_context_, index_count, instance_count, first_index, vertex_offset,
                                first_instance);
        }

        // Multi-draw indirect draw call.

        /**
         * @brief Issues multiple indirect draw commands.
         *
         * @param mode The draw mode.
         * @param type The type of the draw command.
         * @param indirect The pointer to the indirect draw commands.
         * @param draw_count The number of draw commands.
         * @param stride The stride between draw commands.
         */
        void multi_draw_indirect(Uint32 mode, Uint32 type, const void *indirect, Uint32 draw_count, Uint32 stride) {
            funcs_.multi_draw_indirect(backend_context_, mode, type, indirect, draw_count, stride);
        }

        /**
         * @brief Issues multiple indirect draw commands for non-indexed drawing.
         *
         * @param mode The draw mode.
         * @param indirect The pointer to the indirect draw commands.
         * @param draw_count The number of draw commands.
         * @param stride The stride between draw commands.
         */
        void multi_draw_arrays_indirect(Uint32 mode, const void *indirect, Uint32 draw_count, Uint32 stride) {
            funcs_.multi_draw_arrays_indirect(backend_context_, mode, indirect, draw_count, stride);
        }

        // Buffer mapping functions.

        /**
         * @brief Maps a buffer to CPU accessible memory.
         *
         * @param handle The handle to the buffer to map.
         * @param offset The offset within the buffer.
         * @param size The size of the memory to map.
         * @param flags The access flags for the mapping.
         * @return A pointer to the mapped memory.
         */
        void *map_buffer(BufferHandle handle, Uint32 offset, Uint32 size, AccessFlags flags) {
            return funcs_.map_buffer(backend_context_, handle, offset, size, flags);
        }

        /**
         * @brief Unmaps a buffer from CPU accessible memory.
         *
         * @param handle The handle to the buffer to unmap.
         */
        void unmap_buffer(BufferHandle handle) {
            funcs_.unmap_buffer(backend_context_, handle);
        }

        /**
         * @brief Updates the contents of a buffer.
         *
         * @param handle The handle to the buffer to update.
         * @param data The pointer to the data to update the buffer with.
         * @param offset The offset within the buffer.
         * @param size The size of the data to update.
         */
        void update_buffer(BufferHandle handle, const void *data, Uint32 offset, Uint32 size) {
            funcs_.update_buffer(backend_context_, handle, data, offset, size);
        }

        // Compute dispatch commands

        /**
         * @brief Dispatches a compute command.
         *
         * @param group_count_x The number of work groups in the X dimension.
         * @param group_count_y The number of work groups in the Y dimension.
         * @param group_count_z The number of work groups in the Z dimension.
         */
        void dispatch(Uint32 group_count_x, Uint32 group_count_y, Uint32 group_count_z) {
            funcs_.dispatch(backend_context_, group_count_x, group_count_y, group_count_z);
        }

        /**
         * @brief Dispatches an indirect compute command.
         *
         * @param cmd The handle to the command buffer.
         * @param indirect_buffer The handle to the buffer containing the indirect command.
         * @param offset The offset within the buffer.
         */
        void dispatch_indirect(CommandBufferHandle cmd, BufferHandle indirect_buffer, Uint32 offset) {
            funcs_.dispatch_indirect(backend_context_, cmd, indirect_buffer, offset);
        }

        // Pipeline binding and dynamic state commands

        /**
         * @brief Binds a pipeline to a command buffer.
         *
         * @param cmd The handle to the command buffer.
         * @param pipeline The handle to the pipeline to bind.
         */
        void bind_pipeline(CommandBufferHandle cmd, PipelineHandle pipeline) {
            funcs_.bind_pipeline(backend_context_, cmd, pipeline);
        }

        /**
         * @brief Binds vertex buffers to a command buffer.
         *
         * @param cmd The handle to the command buffer.
         * @param first_binding The first binding index.
         * @param buffers The list of buffer handles to bind.
         * @param offsets The list of offsets within the buffers.
         */
        void
        bind_vertex_buffers(CommandBufferHandle cmd, Uint32 first_binding, const std::vector<BufferHandle> &buffers,
                            const std::vector<Uint32> &offsets) {
            funcs_.bind_vertex_buffers(backend_context_, cmd, first_binding, buffers.data(), offsets.data(),
                                       static_cast<Uint32>(buffers.size()));
        }

        /**
         * @brief Binds an index buffer to a command buffer.
         *
         * @param cmd The handle to the command buffer.
         * @param buffer The handle to the index buffer.
         * @param offset The offset within the buffer.
         * @param index_type The type of the index buffer.
         */
        void bind_index_buffer(CommandBufferHandle cmd, BufferHandle buffer, Uint32 offset, IndexType index_type) {
            funcs_.bind_index_buffer(backend_context_, cmd, buffer, offset, index_type);
        }

        /**
         * @brief Sets the viewport state.
         *
         * @param cmd The handle to the command buffer.
         * @param viewport The viewport state to set.
         */
        void set_viewport(CommandBufferHandle cmd, const Viewport &viewport) {
            funcs_.set_viewport(backend_context_, cmd, viewport);
        }

        /**
         * @brief Sets the scissor state.
         *
         * @param cmd The handle to the command buffer.
         * @param rect The scissor rectangle to set.
         */
        void set_scissor(CommandBufferHandle cmd, const Rect2D &rect) {
            funcs_.set_scissor(backend_context_, cmd, rect);
        }

        /**
         * @brief Updates the push constants for a command buffer.
         *
         * This function updates the push constants for a given command buffer.
         *
         * @param cmd The handle to the command buffer.
         * @param layout The handle to the pipeline layout.
         * @param stage The shader stage that will use the push constants.
         * @param offset The offset within the push constants.
         * @param size The size of the push constants.
         * @param data The pointer to the data to update the push constants with.
         */
        void
        update_push_constants(CommandBufferHandle cmd, PipelineLayoutHandle layout, ShaderStage stage, Uint32 offset,
                              Uint32 size, const void *data) {
            funcs_.update_push_constants(backend_context_, cmd, layout, stage, offset, size, data);
        }

        /**
         * @brief Copies data from one buffer to another.
         *
         * This function copies data from a source buffer to a destination buffer.
         *
         * @param src The handle to the source buffer.
         * @param dst The handle to the destination buffer.
         * @param src_offset The offset within the source buffer.
         * @param dst_offset The offset within the destination buffer.
         * @param size The size of the data to copy.
         */
        void copy_buffer(BufferHandle src, BufferHandle dst, Uint32 src_offset, Uint32 dst_offset, Uint32 size) {
            funcs_.copy_buffer(backend_context_, src, dst, src_offset, dst_offset, size);
        }

        /**
         * @brief Copies data from one texture to another.
         *
         * This function copies data from a source texture to a destination texture.
         *
         * @param src The handle to the source texture.
         * @param dst The handle to the destination texture.
         */
        void copy_texture(TextureHandle src, TextureHandle dst) {
            funcs_.copy_texture(backend_context_, src, dst);
        }

        /**
         * @brief Resets a query pool.
         *
         * This function resets a range of queries in a query pool.
         *
         * @param pool The handle to the query pool.
         * @param first_query The first query to reset.
         * @param query_count The number of queries to reset.
         */
        void reset_query_pool(QueryPoolHandle pool, Uint32 first_query, Uint32 query_count) {
            funcs_.reset_query_pool(backend_context_, pool, first_query, query_count);
        }

        /**
         * @brief Begins a query.
         *
         * This function begins a query in a command buffer.
         *
         * @param cmd The handle to the command buffer.
         * @param pool The handle to the query pool.
         * @param query The index of the query to begin.
         * @param name The name of the query.
         */
        void begin_query(CommandBufferHandle cmd, QueryPoolHandle pool, Uint32 query, const char *name) {
            funcs_.begin_query(backend_context_, cmd, pool, query, name);
        }

        /**
         * @brief Ends a query.
         *
         * This function ends a query in a command buffer.
         *
         * @param cmd The handle to the command buffer.
         * @param pool The handle to the query pool.
         * @param query The index of the query to end.
         */
        void end_query(CommandBufferHandle cmd, QueryPoolHandle pool, Uint32 query) {
            funcs_.end_query(backend_context_, cmd, pool, query);
        }

        /**
         * @brief Retrieves the results of queries.
         *
         * This function retrieves the results of a range of queries from a query pool.
         *
         * @param pool The handle to the query pool.
         * @param first_query The first query to retrieve results from.
         * @param query_count The number of queries to retrieve results from.
         * @param results The pointer to the memory to store the results.
         * @param stride The stride between results.
         * @param wait Whether to wait for the results to be available.
         */
        void
        get_query_results(QueryPoolHandle pool, Uint32 first_query, Uint32 query_count, void *results, Uint32 stride,
                          bool wait) {
            funcs_.get_query_results(backend_context_, pool, first_query, query_count, results, stride, wait);
        }

        /**
         * @brief Begins a debug marker region.
         *
         * This function begins a debug marker region in a command buffer.
         *
         * @param cmd The handle to the command buffer.
         * @param marker_name The name of the debug marker.
         * @param color The color of the debug marker.
         */
        void begin_debug_marker(CommandBufferHandle cmd, const char *marker_name, const float color[4]) {
            funcs_.begin_debug_marker(backend_context_, cmd, marker_name, color);
        }

        /**
         * @brief Ends a debug marker region.
         *
         * This function ends a debug marker region in a command buffer.
         *
         * @param cmd The handle to the command buffer.
         */
        void end_debug_marker(CommandBufferHandle cmd) {
            funcs_.end_debug_marker(backend_context_, cmd);
        }

        /**
         * @brief Inserts a debug marker.
         *
         * This function inserts a debug marker in a command buffer.
         *
         * @param cmd The handle to the command buffer.
         * @param marker_name The name of the debug marker.
         * @param color The color of the debug marker.
         */
        void insert_debug_marker(CommandBufferHandle cmd, const char *marker_name, const float color[4]) {
            funcs_.insert_debug_marker(backend_context_, cmd, marker_name, color);
        }

        // ---------------------------
        // Pipeline layout creation/destruction
        // ---------------------------
        /**
         * @brief Creates a pipeline layout.
         *
         * This function creates a pipeline layout.
         *
         * @param desc The description of the pipeline layout to create.
         * @return The handle to the created pipeline layout.
         */
        PipelineLayoutHandle create_pipeline_layout(const PipelineLayoutDesc &desc) {
            return funcs_.create_pipeline_layout(backend_context_, desc);
        }

        /**
         * @brief Destroys a pipeline layout.
         *
         * This function destroys a pipeline layout.
         *
         * @param layout The handle to the pipeline layout to destroy.
         */
        void destroy_pipeline_layout(PipelineLayoutHandle layout) {
            funcs_.destroy_pipeline_layout(backend_context_, layout);
        }

        /**
         * @brief Creates a compute pipeline.
         *
         * This function creates a compute pipeline.
         *
         * @param desc The description of the compute pipeline to create.
         * @return The handle to the created compute pipeline.
         */
        PipelineHandle create_compute_pipeline(const ComputePipelineDesc &desc) {
            return funcs_.create_compute_pipeline(backend_context_, desc);
        }

        /**
         * @brief Destroys a compute pipeline.
         *
         * This function destroys a compute pipeline.
         *
         * @param handle The handle to the compute pipeline to destroy.
         */
        void destroy_compute_pipeline(PipelineHandle handle) {
            funcs_.destroy_compute_pipeline(backend_context_, handle);
        }

        /**
         * @brief Creates a mesh pipeline.
         *
         * This function creates a mesh pipeline.
         *
         * @param desc The description of the mesh pipeline to create.
         * @return The handle to the created mesh pipeline.
         */
        PipelineHandle create_mesh_pipeline(const MeshPipelineDesc &desc) {
            return funcs_.create_mesh_pipeline(backend_context_, desc);
        }

        /**
         * @brief Destroys a mesh pipeline.
         *
         * This function destroys a mesh pipeline.
         *
         * @param handle The handle to the mesh pipeline to destroy.
         */
        void destroy_mesh_pipeline(PipelineHandle handle) {
            funcs_.destroy_mesh_pipeline(backend_context_, handle);
        }

        /**
         * @brief Creates a texture view.
         *
         * This function creates a texture view.
         *
         * @param desc The description of the texture view to create.
         * @return The handle to the created texture view.
         */
        TextureViewHandle create_texture_view(const TextureViewDesc &desc) {
            return funcs_.create_texture_view(backend_context_, desc);
        }

        /**
         * @brief Destroys a texture view.
         *
         * This function destroys a texture view.
         *
         * @param handle The handle to the texture view to destroy.
         */
        void destroy_texture_view(TextureViewHandle handle) {
            funcs_.destroy_texture_view(backend_context_, handle);
        }

        /**
         * @brief Begins a render pass.
         *
         * This function begins a render pass in a command buffer.
         *
         * @param cmd The handle to the command buffer.
         * @param begin_info The information required to begin the render pass.
         */
        void begin_render_pass(CommandBufferHandle cmd, const RenderPassBeginInfo &begin_info) {
            funcs_.begin_render_pass(backend_context_, cmd, begin_info);
        }

        /**
         * @brief Ends a render pass.
         *
         * This function ends a render pass in a command buffer.
         *
         * @param cmd The handle to the command buffer.
         */
        void end_render_pass(CommandBufferHandle cmd) {
            funcs_.end_render_pass(backend_context_, cmd);
        }

        /**
         * @brief Gets the type of the backend.
         *
         * This function returns the type of the backend.
         *
         * @return The type of the backend.
         */
        [[nodiscard]] BackendType get_backend_type() const { return backend_type_; }

        /**
         * @brief Gets the context of the backend.
         *
         * This function returns the context of the backend.
         *
         * @return The context of the backend.
         */
        [[nodiscard]] void *get_backend_context() const { return backend_context_; }

    protected:
        DeviceFunctions funcs_; ///< The structure containing function pointers for device operations.
        void *backend_context_ = nullptr; ///< The context for the backend implementation.
        BackendType backend_type_; ///< The type of the backend.
    };
} // namespace Bcg::Graphics
