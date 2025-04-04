#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <variant> // For ClearValue

// Assuming GraphicsTypes.h defines these base types, or define them here.
// If not defined elsewhere, uncomment these lines:
// using Uint8 = uint8_t;
// using Uint16 = uint16_t;
// using Uint32 = uint32_t;
// using Uint64 = uint64_t;
// using Int32 = int32_t;
// using SizeType = size_t; // Or std::size_t
// using Bool32 = Uint32;   // For API alignment if needed

namespace Bcg::Graphics {

    // Forward Declarations
    struct DeviceFunctions;

    class Device;

    // Base Types (Alias for clarity if not defined elsewhere)
    using Uint8 = uint8_t;
    using Uint16 = uint16_t;
    using Uint32 = uint32_t;
    using Uint64 = uint64_t;
    using Int32 = int32_t;
    using SizeType = size_t;
    using Bool32 = Uint32; // Common practice in Vulkan for bools in structs

    // ----------------------------------------------------------
    // Backend Type Enum
    // ----------------------------------------------------------
    enum class BackendType {
        Unknown,
        Vulkan,
        OpenGL,     // Might need separate Core/ES profiles
        Direct3D12,
        Metal
    };

    // ----------------------------------------------------------
    // Handle Types (Opaque Identifiers)
    // ----------------------------------------------------------
    // Notes:
    // - Using simple structs with IDs. Consider `enum class` or more complex handle systems
    //   for type safety if needed.
    // - InvalidId is 0 for simplicity.

    static constexpr Uint32 InvalidId32 = 0;
    static constexpr Uint64 InvalidId64 = 0;

    struct BufferHandle {
        Uint32 id = InvalidId32;
    };
    struct TextureHandle {
        Uint32 id = InvalidId32;
    };
    struct TextureViewHandle {
        Uint32 id = InvalidId32;
    }; // Added
    struct SamplerHandle {
        Uint32 id = InvalidId32;
    };
    struct ShaderHandle {
        Uint32 id = InvalidId32;
    };
    struct PipelineLayoutHandle {
        Uint32 id = InvalidId32;
    }; // Added
    struct PipelineHandle {
        Uint32 id = InvalidId32;
    }; // Graphics, Compute, Mesh, RT
    struct RenderPassHandle {
        Uint32 id = InvalidId32;
    };
    struct FramebufferHandle {
        Uint32 id = InvalidId32;
    };
    struct DescriptorSetLayoutHandle {
        Uint32 id = InvalidId32;
    };
    struct DescriptorSetHandle {
        Uint32 id = InvalidId32;
    };
    struct CommandBufferHandle {
        Uint32 id = InvalidId32;
    };
    struct FenceHandle {
        Uint64 id = InvalidId64;
    }; // Use 64-bit for potential pointer packing
    struct SemaphoreHandle {
        Uint64 id = InvalidId64;
    }; // Use 64-bit
    struct SwapchainHandle {
        Uint32 id = InvalidId32;
    };
    struct QueryPoolHandle {
        Uint32 id = InvalidId32;
    }; // Added
    struct PipelineCacheHandle {
        Uint32 id = InvalidId32;
    }; // Added
    struct SurfaceHandle {
        Uint64 id = InvalidId64;
    }; // Added, likely platform-specific pointer/handle
    struct AccelerationStructureHandle {
        Uint64 id = InvalidId64;
    }; // Added (RT)
    // struct RayTracingPipelineHandle { Uint32 id = InvalidId32; }; // Alias PipelineHandle or separate? Use PipelineHandle for now.

    // Handle validation helpers
    inline bool is_valid(BufferHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(TextureHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(TextureViewHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(SamplerHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(ShaderHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(PipelineLayoutHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(PipelineHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(RenderPassHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(FramebufferHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(DescriptorSetLayoutHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(DescriptorSetHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(CommandBufferHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(FenceHandle h) { return h.id != InvalidId64; }

    inline bool is_valid(SemaphoreHandle h) { return h.id != InvalidId64; }

    inline bool is_valid(SwapchainHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(QueryPoolHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(PipelineCacheHandle h) { return h.id != InvalidId32; }

    inline bool is_valid(SurfaceHandle h) { return h.id != InvalidId64; }

    inline bool is_valid(AccelerationStructureHandle h) { return h.id != InvalidId64; }


    // ----------------------------------------------------------
    // Enums
    // ----------------------------------------------------------

    enum class Format {
        Undefined,
        // 8-bit formats
        R8_UNorm, R8_SNorm, R8_UInt, R8_SInt,
        RG8_UNorm, RG8_SNorm, RG8_UInt, RG8_SInt,
        // No RGB8 - Often unsupported directly on GPU. Emulate or use RGBA8.
        RGBA8_UNorm, RGBA8_SNorm, RGBA8_UInt, RGBA8_SInt,
        BGRA8_UNorm, // Common for swapchains
        SRGBA8_UNorm, // sRGB
        SBGRA8_UNorm, // sRGB
        // 16-bit formats
        R16_UNorm, R16_SNorm, R16_UInt, R16_SInt, R16_SFloat,
        RG16_UNorm, RG16_SNorm, RG16_UInt, RG16_SInt, RG16_SFloat,
        RGBA16_UNorm, RGBA16_SNorm, RGBA16_UInt, RGBA16_SInt, RGBA16_SFloat,
        // 32-bit formats
        R32_UInt, R32_SInt, R32_SFloat,
        RG32_UInt, RG32_SInt, RG32_SFloat,
        RGB32_UInt, RGB32_SInt, RGB32_SFloat,
        RGBA32_UInt, RGBA32_SInt, RGBA32_SFloat,
        // Depth/stencil formats
        D16_UNorm,
        D32_SFloat,
        S8_UInt,
        D24_UNorm_S8_UInt,
        D32_SFloat_S8_UInt, // Optional S8 part
        // Compressed formats (BCn / DXT) - Add as needed
        BC1_RGB_UNorm, BC1_RGBA_UNorm, BC2_UNorm, BC3_UNorm, // DXT1, DXT3, DXT5
        BC4_UNorm, BC4_SNorm, // ATI1N
        BC5_UNorm, BC5_SNorm, // ATI2N
        BC6H_UFloat, BC6H_SFloat,
        BC7_UNorm,
        BC1_SRGB, BC1_SRGBA, BC2_SRGB, BC3_SRGB, BC7_SRGB, // sRGB variants
        // ASTC, ETC formats - Add if targeting mobile primarily
    };

    // Changed to Flags for bitmask usage
    enum class ShaderStageFlags : Uint32 {
        None = 0,
        Vertex = 1 << 0,
        Fragment = 1 << 1,
        Compute = 1 << 2,
        Geometry = 1 << 3,
        TessellationControl = 1 << 4,
        TessellationEvaluation = 1 << 5,
        Task = 1 << 6,  // Mesh Shading
        Mesh = 1 << 7,  // Mesh Shading
        RayGen = 1 << 8,  // Ray Tracing
        AnyHit = 1 << 9,  // Ray Tracing
        ClosestHit = 1 << 10, // Ray Tracing
        Miss = 1 << 11, // Ray Tracing
        Intersection = 1 << 12, // Ray Tracing
        Callable = 1 << 13, // Ray Tracing
        AllGraphics = Vertex | Fragment | Geometry | TessellationControl | TessellationEvaluation | Task | Mesh,
        AllStages = 0xFFFFFFFF // Or combine all defined flags
    };

    // Enable bitmask operators for ShaderStageFlags
    inline ShaderStageFlags operator|(ShaderStageFlags a, ShaderStageFlags b) {
        return static_cast<ShaderStageFlags>(static_cast<Uint32>(a) | static_cast<Uint32>(b));
    }

    inline ShaderStageFlags operator&(ShaderStageFlags a, ShaderStageFlags b) {
        return static_cast<ShaderStageFlags>(static_cast<Uint32>(a) & static_cast<Uint32>(b));
    }

    inline ShaderStageFlags
    operator~(ShaderStageFlags a) { return static_cast<ShaderStageFlags>(~static_cast<Uint32>(a)); }

    inline ShaderStageFlags &operator|=(ShaderStageFlags &a, ShaderStageFlags b) {
        a = a | b;
        return a;
    }

    inline ShaderStageFlags &operator&=(ShaderStageFlags &a, ShaderStageFlags b) {
        a = a & b;
        return a;
    }


    enum class Filter {
        Nearest, Linear
    };
    enum class MipmapMode {
        Nearest, Linear
    };
    enum class AddressMode {
        Repeat, MirroredRepeat, ClampToEdge, ClampToBorder, MirrorClampToEdge
    };
    enum class BorderColor {
        FloatTransparentBlack, IntTransparentBlack, FloatOpaqueBlack, IntOpaqueBlack, FloatOpaqueWhite, IntOpaqueWhite
    };
    enum class CompareOp {
        Never, Less, Equal, LessOrEqual, Greater, NotEqual, GreaterOrEqual, Always
    };

    enum class BlendFactor {
        Zero, One,
        SrcColor, OneMinusSrcColor, DstColor, OneMinusDstColor,
        SrcAlpha, OneMinusSrcAlpha, DstAlpha, OneMinusDstAlpha,
        ConstantColor, OneMinusConstantColor, ConstantAlpha, OneMinusConstantAlpha,
        SrcAlphaSaturate,
        Src1Color, OneMinusSrc1Color, Src1Alpha, OneMinusSrc1Alpha // For dual source blending
    };
    enum class BlendOp {
        Add, Subtract, ReverseSubtract, Min, Max
    };
    enum class LogicOp { // Added
        Clear,
        And,
        AndReverse,
        Copy,
        AndInverted,
        NoOp,
        Xor,
        Or,
        Nor,
        Equivalent,
        Invert,
        OrReverse,
        CopyInverted,
        OrInverted,
        Nand,
        Set
    };
    enum class ColorComponentFlags : Uint8 { // Added bitmask
        None = 0, R = 1 << 0, G = 1 << 1, B = 1 << 2, A = 1 << 3, All = R | G | B | A
    };

    inline ColorComponentFlags operator|(ColorComponentFlags a, ColorComponentFlags b) {
        return static_cast<ColorComponentFlags>(static_cast<Uint8>(a) | static_cast<Uint8>(b));
    }

    enum class PrimitiveTopology { // Added Enum
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip,
        TriangleFan, // Note: Often less efficient or deprecated in modern APIs
        LineListWithAdjacency,
        LineStripWithAdjacency,
        TriangleListWithAdjacency,
        TriangleStripWithAdjacency,
        PatchList // For Tessellation
    };

    enum class PolygonMode {
        Fill, Line, Point
    };
    enum class CullMode {
        None, Front, Back, FrontAndBack
    };
    enum class FrontFace {
        CounterClockwise, Clockwise
    };

    enum class LoadOp {
        Load, Clear, DontCare
    };
    enum class StoreOp {
        Store, DontCare
    }; // Maybe StoreOnly?

    enum class MemoryUsage {
        GpuOnly,        // Device local, host invisible
        CpuOnly,        // Host visible, coherent, maybe cached (slow for GPU)
        CpuToGpu,       // Host visible, device local preferred, write-combined/uncached
        GpuToCpu,       // Host visible, cached, coherent (readback)
        Lazy // Memory allocation can be deferred until needed (e.g., transient attachments)
    };

    enum class BufferUsageFlags : Uint32 { // Changed to Flags
        None = 0,
        TransferSrc = 1 << 0,
        TransferDst = 1 << 1,
        VertexBuffer = 1 << 2,
        IndexBuffer = 1 << 3,
        UniformBuffer = 1 << 4,
        StorageBuffer = 1 << 5,
        IndirectBuffer = 1 << 6,
        ShaderDeviceAddress = 1 << 7, // For Bindless / Buffer Pointers
        AccelerationStructureBuildInput = 1 << 8, // RT
        AccelerationStructureStorage = 1 << 9     // RT
    };

    inline BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b) {
        return static_cast<BufferUsageFlags>(static_cast<Uint32>(a) | static_cast<Uint32>(b));
    }

    inline BufferUsageFlags &operator|=(BufferUsageFlags &a, BufferUsageFlags b) {
        a = a | b;
        return a;
    }

    enum class TextureUsageFlags : Uint32 { // Changed to Flags
        None = 0,
        TransferSrc = 1 << 0,
        TransferDst = 1 << 1,
        Sampled = 1 << 2,
        Storage = 1 << 3,
        ColorAttachment = 1 << 4,
        DepthStencilAttachment = 1 << 5,
        TransientAttachment = 1 << 6, // Attachment whose contents don't need to persist
        InputAttachment = 1 << 7
    };

    inline TextureUsageFlags operator|(TextureUsageFlags a, TextureUsageFlags b) {
        return static_cast<TextureUsageFlags>(static_cast<Uint32>(a) | static_cast<Uint32>(b));
    }

    inline TextureUsageFlags &operator|=(TextureUsageFlags &a, TextureUsageFlags b) {
        a = a | b;
        return a;
    }


    enum class TextureType {
        Texture1D, Texture2D, Texture3D, CubeMap, Texture1DArray, Texture2DArray, CubeMapArray
    };
    enum class TextureViewType {
        View1D, View2D, View3D, ViewCube, View1DArray, View2DArray, ViewCubeArray
    }; // Added for views

    enum class ImageLayout { // Added
        Undefined,                  // Initial state, cannot be used directly
        General,                    // Supports all access types (often less optimal)
        ColorAttachmentOptimal,     // Optimal for color attachment read/write
        DepthStencilAttachmentOptimal, // Optimal for depth/stencil read/write
        DepthStencilReadOnlyOptimal, // Optimal for depth/stencil read-only (e.g., shader sample)
        ShaderReadOnlyOptimal,      // Optimal for shader sampling/uniform reads
        TransferSrcOptimal,         // Optimal as source for copies
        TransferDstOptimal,         // Optimal as destination for copies
        Preinitialized,             // Initial state for host-visible memory before first transition
        PresentSrc                  // Optimal for presenting to screen
    };

    enum class PresentMode {
        Immediate, Mailbox, Fifo, FifoRelaxed
    };

    enum class SampleCount : Uint8 {
        Count1 = 1, Count2 = 2, Count4 = 4, Count8 = 8, Count16 = 16, Count32 = 32, Count64 = 64
    };

    // Changed to Flags for bitmask usage
    enum class PipelineStageFlags : Uint32 {
        None = 0,
        TopOfPipe = 1 << 0,                 // Start of the pipeline
        DrawIndirect = 1 << 1,              // Indirect buffer reading
        VertexInput = 1 << 2,               // Vertex/Index buffer reading
        VertexShader = 1 << 3,
        TessellationControlShader = 1 << 4,
        TessellationEvaluationShader = 1 << 5,
        GeometryShader = 1 << 6,
        FragmentShader = 1 << 7,
        EarlyFragmentTests = 1 << 8,          // Depth/Stencil tests before Fragment Shader
        LateFragmentTests = 1 << 9,           // Depth/Stencil tests after Fragment Shader
        ColorAttachmentOutput = 1 << 10,      // Color attachment writes
        ComputeShader = 1 << 11,
        Transfer = 1 << 12,                 // Copy/Blit/Resolve/Clear commands
        BottomOfPipe = 1 << 13,               // End of the pipeline
        Host = 1 << 14,                     // Host CPU access
        TaskShader = 1 << 15,               // Mesh Shading
        MeshShader = 1 << 16,               // Mesh Shading
        RayTracingShader = 1 << 17,         // RT
        AccelerationStructureBuild = 1 << 18, // RT
        AllGraphics =
        DrawIndirect | VertexInput | VertexShader | TessellationControlShader | TessellationEvaluationShader |
        GeometryShader | FragmentShader | EarlyFragmentTests | LateFragmentTests | ColorAttachmentOutput | TaskShader |
        MeshShader,
        AllCommands = AllGraphics | ComputeShader | Transfer | RayTracingShader | AccelerationStructureBuild,
        All = 0xFFFFFFFF
    };

    inline PipelineStageFlags operator|(PipelineStageFlags a, PipelineStageFlags b) {
        return static_cast<PipelineStageFlags>(static_cast<Uint32>(a) | static_cast<Uint32>(b));
    }

    inline PipelineStageFlags operator&(PipelineStageFlags a, PipelineStageFlags b) {
        return static_cast<PipelineStageFlags>(static_cast<Uint32>(a) & static_cast<Uint32>(b));
    }

    inline PipelineStageFlags
    operator~(PipelineStageFlags a) { return static_cast<PipelineStageFlags>(~static_cast<Uint32>(a)); }

    inline PipelineStageFlags &operator|=(PipelineStageFlags &a, PipelineStageFlags b) {
        a = a | b;
        return a;
    }

    inline PipelineStageFlags &operator&=(PipelineStageFlags &a, PipelineStageFlags b) {
        a = a & b;
        return a;
    }


    // Changed to Flags for bitmask usage
    enum class AccessFlags : Uint32 {
        None = 0,
        IndirectCommandRead = 1 << 0,
        IndexRead = 1 << 1,
        VertexAttributeRead = 1 << 2,
        UniformRead = 1 << 3,
        InputAttachmentRead = 1 << 4,
        ShaderRead = 1 << 5,
        ShaderWrite = 1 << 6,
        ColorAttachmentRead = 1 << 7,
        ColorAttachmentWrite = 1 << 8,
        DepthStencilAttachmentRead = 1 << 9,
        DepthStencilAttachmentWrite = 1 << 10,
        TransferRead = 1 << 11,
        TransferWrite = 1 << 12,
        HostRead = 1 << 13,
        HostWrite = 1 << 14,
        MemoryRead = 1 << 15,  // Generic read
        MemoryWrite = 1 << 16, // Generic write
        AccelerationStructureRead = 1 << 17, // RT
        AccelerationStructureWrite = 1 << 18 // RT
    };

    inline AccessFlags operator|(AccessFlags a, AccessFlags b) {
        return static_cast<AccessFlags>(static_cast<Uint32>(a) | static_cast<Uint32>(b));
    }

    inline AccessFlags operator&(AccessFlags a, AccessFlags b) {
        return static_cast<AccessFlags>(static_cast<Uint32>(a) & static_cast<Uint32>(b));
    }

    inline AccessFlags operator~(AccessFlags a) { return static_cast<AccessFlags>(~static_cast<Uint32>(a)); }

    inline AccessFlags &operator|=(AccessFlags &a, AccessFlags b) {
        a = a | b;
        return a;
    }

    inline AccessFlags &operator&=(AccessFlags &a, AccessFlags b) {
        a = a & b;
        return a;
    }


    enum class DeviceFeature {
        // Core
        SamplerAnisotropy,
        TextureCompressionBC, // BC1-7
        TextureCompressionETC2, // Mobile
        TextureCompressionASTC, // Mobile
        ShaderFloat64,
        ShaderInt64,
        ShaderInt16,
        // Geometry/Tessellation
        GeometryShaders,
        TessellationShaders,
        // Pipeline
        MultiDrawIndirect,
        DrawIndirectFirstInstance,
        DepthClamp,
        DepthBiasClamp,
        FillModeNonSolid, // Line/Point rendering
        DepthBounds, // Min/Max depth bounds test
        WideLines, // Lines thicker than 1.0
        LargePoints, // Points larger than 1.0
        AlphaToOne,
        DualSourceBlending,
        LogicOp,
        IndependentBlend, // Per-attachment blend state
        SampleRateShading, // Per-sample fragment shading
        ConservativeRasterization, // Added
        // Sync/Resources
        MultiViewport, // Rendering to multiple viewports simultaneously
        DescriptorIndexing, // Bindless-like features
        BufferDeviceAddress, // Getting GPU pointers to buffers
        TimelineSemaphores, // Advanced synchronization
        // Compute/Mesh
        ComputeShaders,
        MeshShaders, // Added
        TaskShaders, // Added
        // Ray Tracing
        RayTracing, // Added (umbrella feature)
        AccelerationStructure, // Added
        RayQuery // Added (shader queries without RT pipeline)
    };

    enum class DeviceErrorType {
        None,
        OutOfHostMemory,
        OutOfDeviceMemory,
        InitializationFailed,
        DeviceLost,
        MemoryMapFailed,
        LayerNotPresent,
        ExtensionNotPresent,
        FeatureNotPresent,
        IncompatibleDriver,
        TooManyObjects,
        FormatNotSupported,
        SurfaceLost, // Swapchain related
        Unknown
    };

    enum class DescriptorType {
        Sampler,
        CombinedImageSampler, // TextureView + Sampler
        SampledImage,         // TextureView only (shader uses separate sampler)
        StorageImage,         // TextureView for load/store
        UniformTexelBuffer,   // Formatted BufferView
        StorageTexelBuffer,   // Formatted BufferView
        UniformBuffer,        // Buffer for UBOs
        StorageBuffer,        // Buffer for SSBOs
        UniformBufferDynamic, // UBO with dynamic offset at bind time
        StorageBufferDynamic, // SSBO with dynamic offset at bind time
        InputAttachment,      // Read from framebuffer attachment in shader
        AccelerationStructure // RT
    };

    enum class IndexType {
        UInt16, UInt32
    };
    enum class QueryType {
        Occlusion, PipelineStatistics, Timestamp
    }; // Added
    enum class ShaderLanguage {
        GLSL, HLSL, SPIRV, MSL, WGSL
    }; // Added

    // ----------------------------------------------------------
    // Descriptors for Resource Creation
    // ----------------------------------------------------------

    struct BufferDesc {
        SizeType size = 0;
        MemoryUsage memory_usage = MemoryUsage::GpuOnly;
        BufferUsageFlags usage = BufferUsageFlags::None;
        // Optional: Sharing mode if using multiple queue families
    };

    struct TextureDesc {
        TextureType type = TextureType::Texture2D;
        Format format = Format::Undefined;
        Uint32 width = 0;
        Uint32 height = 0;
        Uint32 depth_or_layers = 1; // Depth for 3D, Layers for Array/Cube
        Uint32 mip_levels = 1;
        SampleCount sample_count = SampleCount::Count1;
        TextureUsageFlags usage = TextureUsageFlags::None;
        MemoryUsage memory_usage = MemoryUsage::GpuOnly;
        ImageLayout initial_layout = ImageLayout::Undefined; // Added
        // Optional: Sharing mode, tiling (optimal/linear)
    };

    struct TextureViewDesc { // Added TextureViewDesc
        TextureHandle texture = {};
        TextureViewType view_type = TextureViewType::View2D; // Must be compatible with texture type
        Format format = Format::Undefined; // Can reinterpret format if compatible
        ColorComponentFlags component_swizzle_r = ColorComponentFlags::R; // Example swizzle
        ColorComponentFlags component_swizzle_g = ColorComponentFlags::G;
        ColorComponentFlags component_swizzle_b = ColorComponentFlags::B;
        ColorComponentFlags component_swizzle_a = ColorComponentFlags::A;
        Uint32 base_mip_level = 0;
        Uint32 level_count = 1; // Use RemainingMipLevels?
        Uint32 base_array_layer = 0;
        Uint32 layer_count = 1; // Use RemainingArrayLayers?
    };

    struct SamplerDesc {
        Filter mag_filter = Filter::Linear;
        Filter min_filter = Filter::Linear;
        MipmapMode mipmap_mode = MipmapMode::Linear;
        AddressMode address_mode_u = AddressMode::Repeat;
        AddressMode address_mode_v = AddressMode::Repeat;
        AddressMode address_mode_w = AddressMode::Repeat;
        float mip_lod_bias = 0.0f;
        Bool32 anisotropy_enable = false; // Use Bool32 for struct packing
        float max_anisotropy = 1.0f; // Often 16.0 is max HW supports
        Bool32 compare_enable = false; // For depth comparison samplers
        CompareOp compare_op = CompareOp::Never;
        float min_lod = 0.0f;
        float max_lod = 1000.0f; // Or a sentinel like FLT_MAX
        BorderColor border_color = BorderColor::FloatTransparentBlack;
        // Bool32 unnormalized_coordinates = false; // Less common
    };

    struct ShaderDesc {
        ShaderStageFlags stage; // Now a flag (though only one stage per module usually)
        ShaderLanguage language = ShaderLanguage::SPIRV; // Added
        const void *code = nullptr;
        SizeType code_size = 0;
        const char *entry_point = "main";
        // std::vector<SpecializationConstant> constants; // Optional SPIR-V specialization constants
        // Removed macros, optimize, debug_info - Often handled by offline compilers or backend flags
    };

    // Input Assembler State
    struct InputBindingDesc {
        Uint32 binding = 0;
        Uint32 stride = 0;
        Bool32 input_rate_instance = false; // true=Instance, false=Vertex
    };

    struct InputAttributeDesc {
        Uint32 location = 0; // Shader location
        Uint32 binding = 0;  // Maps to InputBindingDesc
        Format format = Format::Undefined;
        Uint32 offset = 0;   // Offset within the stride
    };

    struct InputLayoutDesc {
        std::vector<InputBindingDesc> bindings;
        std::vector<InputAttributeDesc> attributes;
    };

    // Primitive Assembly / Rasterization State
    struct RasterizationStateDesc {
        Bool32 depth_clamp_enable = false;
        // Bool32 rasterizer_discard_enable = false; // Use if pipeline has no fragment stage/output
        PolygonMode polygon_mode = PolygonMode::Fill;
        CullMode cull_mode = CullMode::Back;
        FrontFace front_face = FrontFace::CounterClockwise;
        Bool32 depth_bias_enable = false;
        float depth_bias_constant_factor = 0.0f;
        float depth_bias_clamp = 0.0f;
        float depth_bias_slope_factor = 0.0f;
        float line_width = 1.0f; // Requires WideLines feature if > 1.0
        Bool32 conservative_rasterization_enable = false; // Added
    };

    // Depth/Stencil State
    struct StencilOpState { // Renamed from StencilOpDesc for clarity
        BlendOp fail_op = BlendOp::Add; // Should be StencilOp enum? Vulkan uses StencilOp Keep/Zero/Replace etc. Let's add it.
        BlendOp pass_op = BlendOp::Add;
        BlendOp depth_fail_op = BlendOp::Add;
        CompareOp compare_op = CompareOp::Always;
        Uint32 compare_mask = 0xFF;
        Uint32 write_mask = 0xFF;
        Uint32 reference = 0;
    };

    struct DepthStencilStateDesc {
        Bool32 depth_test_enable = true;
        Bool32 depth_write_enable = true;
        CompareOp depth_compare_op = CompareOp::Less;
        Bool32 depth_bounds_test_enable = false; // Requires feature
        Bool32 stencil_test_enable = false;
        StencilOpState front;
        StencilOpState back;
        float min_depth_bounds = 0.0f; // Requires feature
        float max_depth_bounds = 1.0f; // Requires feature
    };

    // Blend State
    struct RenderTargetBlendDesc {
        Bool32 blend_enable = false;
        BlendFactor src_color_blend_factor = BlendFactor::One;
        BlendFactor dst_color_blend_factor = BlendFactor::Zero;
        BlendOp color_blend_op = BlendOp::Add;
        BlendFactor src_alpha_blend_factor = BlendFactor::One;
        BlendFactor dst_alpha_blend_factor = BlendFactor::Zero;
        BlendOp alpha_blend_op = BlendOp::Add;
        ColorComponentFlags color_write_mask = ColorComponentFlags::All;
    };

    struct BlendStateDesc {
        // Bool32 independent_blend_enable = false; // If true, use vector below, else use [0] for all
        std::vector<RenderTargetBlendDesc> render_target_blends; // One per color attachment
        Bool32 logic_op_enable = false; // Requires feature
        LogicOp logic_op = LogicOp::Copy;
        float blend_constants[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // For ConstantColor factors
        // Bool32 alpha_to_coverage_enable = false; // Should be in MultisampleStateDesc
        // Bool32 alpha_to_one_enable = false;      // Should be in MultisampleStateDesc
    };

    // Multisample State (Added)
    struct MultisampleStateDesc {
        SampleCount rasterization_samples = SampleCount::Count1;
        Bool32 sample_shading_enable = false; // Requires feature
        float min_sample_shading = 0.0f; // Fraction (0.0 to 1.0)
        // std::vector<Uint32> sample_mask; // Optional mask
        Bool32 alpha_to_coverage_enable = false;
        Bool32 alpha_to_one_enable = false; // Requires feature
    };

    // Tessellation State (Added)
    struct TessellationStateDesc {
        Uint32 patch_control_points = 3; // e.g., 3 for triangles, 4 for quads
    };

    // Descriptor Set Layout
    struct DescriptorSetLayoutBinding {
        Uint32 binding = 0;
        DescriptorType descriptor_type = DescriptorType::UniformBuffer; // Added!
        Uint32 descriptor_count = 1; // For arrays of descriptors
        ShaderStageFlags stage_flags = ShaderStageFlags::None; // Which stages access this
        // SamplerHandle immutable_sampler = {}; // Optional optimization
    };

    struct DescriptorSetLayoutDesc {
        std::vector<DescriptorSetLayoutBinding> bindings;
        // Optional flags (e.g., PushDescriptorSet, UpdateAfterBind)
    };

    // Pipeline Layout
    struct PushConstantRange {
        ShaderStageFlags stage_flags = ShaderStageFlags::None;
        Uint32 offset = 0;
        Uint32 size = 0;
    };

    struct PipelineLayoutDesc {
        std::vector<DescriptorSetLayoutHandle> descriptor_set_layouts; // Use handles now
        std::vector<PushConstantRange> push_constant_ranges; // Define ranges explicitly
    };

    // Render Pass
    struct AttachmentDescription {
        Format format = Format::Undefined;
        SampleCount sample_count = SampleCount::Count1;
        LoadOp load_op = LoadOp::DontCare;
        StoreOp store_op = StoreOp::DontCare;
        LoadOp stencil_load_op = LoadOp::DontCare;     // Added
        StoreOp stencil_store_op = StoreOp::DontCare;    // Added
        ImageLayout initial_layout = ImageLayout::Undefined; // Added
        ImageLayout final_layout = ImageLayout::Undefined;   // Added
        // Bool32 is_depth_stencil = false; // Infer from format? Or keep explicit? Keep for clarity.
    };

    struct AttachmentReference {
        Uint32 attachment_index = ~0U; // Index into RenderPassDesc::attachments, ~0U means unused
        ImageLayout layout = ImageLayout::Undefined; // Layout during this subpass
    };

    struct SubpassDescription {
        // PipelineType pipeline_type = PipelineType::Graphics; // Might be useful
        std::vector<AttachmentReference> input_attachments;   // Added
        std::vector<AttachmentReference> color_attachments;
        std::vector<AttachmentReference> resolve_attachments; // Added (must match color count if used)
        AttachmentReference depth_stencil_attachment;
        // std::vector<Uint32> preserve_attachments; // Indices not used but whose contents must be preserved
    };

    struct SubpassDependency { // Added
        Uint32 src_subpass = ~0U; // ~0U = External before pass
        Uint32 dst_subpass = ~0U; // ~0U = External after pass
        PipelineStageFlags src_stage_mask = PipelineStageFlags::None;
        PipelineStageFlags dst_stage_mask = PipelineStageFlags::None;
        AccessFlags src_access_mask = AccessFlags::None;
        AccessFlags dst_access_mask = AccessFlags::None;
        // Bool32 by_region = false; // Tiling optimization
    };

    struct RenderPassDesc {
        std::vector<AttachmentDescription> attachments;
        std::vector<SubpassDescription> subpasses;
        std::vector<SubpassDependency> dependencies; // Added
    };

    // Dynamic State (Added)
    enum class DynamicState {
        Viewport, Scissor, LineWidth,
        DepthBias, BlendConstants, DepthBounds,
        StencilCompareMask, StencilWriteMask, StencilReference
        // Add more as needed (e.g., VertexInputBindingStride)
    };

    struct DynamicStateDesc {
        std::vector<DynamicState> dynamic_states;
    };

    // Structure to hold shader module + entry point for pipelines
    struct ShaderStageDesc {
        ShaderHandle shader_module;
        const char *entry_point = "main";
    };

    // Graphics Pipeline
    struct PipelineDesc {
        PipelineLayoutHandle layout; // Use handle
        RenderPassHandle render_pass; // Use handle
        Uint32 subpass_index = 0;

        ShaderStageDesc vertex_shader;
        ShaderStageDesc fragment_shader;
        ShaderStageDesc geometry_shader = {}; // Optional
        ShaderStageDesc tessellation_control_shader = {}; // Optional
        ShaderStageDesc tessellation_evaluation_shader = {}; // Optional

        InputLayoutDesc input_layout;
        PrimitiveTopology topology = PrimitiveTopology::TriangleList; // Added
        RasterizationStateDesc rasterization_state;
        MultisampleStateDesc multisample_state; // Added
        DepthStencilStateDesc depth_stencil_state;
        BlendStateDesc blend_state;
        TessellationStateDesc tessellation_state; // Added (only relevant if TC/TE shaders present)
        DynamicStateDesc dynamic_state; // Added

        // Optional: Pipeline Cache, Base Pipeline Handle for derivation
    };

    // Compute Pipeline
    struct ComputePipelineDesc {
        PipelineLayoutHandle layout; // Use handle
        ShaderStageDesc compute_shader;
        // Optional: Pipeline Cache, Base Pipeline Handle
    };

    // Mesh Pipeline (Added)
    struct MeshPipelineDesc {
        PipelineLayoutHandle layout; // Use handle
        RenderPassHandle render_pass; // Use handle (if rendering)
        Uint32 subpass_index = 0;    // Use handle (if rendering)

        ShaderStageDesc task_shader = {}; // Optional
        ShaderStageDesc mesh_shader;
        ShaderStageDesc fragment_shader = {}; // Optional (if rendering)

        // States relevant if rendering (similar to PipelineDesc, but no InputLayout/Tessellation)
        RasterizationStateDesc rasterization_state; // If rendering
        MultisampleStateDesc multisample_state;     // If rendering
        DepthStencilStateDesc depth_stencil_state; // If rendering
        BlendStateDesc blend_state;             // If rendering
        DynamicStateDesc dynamic_state;         // If rendering

        // Optional: Pipeline Cache, Base Pipeline Handle
    };

    // Ray Tracing Pipeline (Added - Simplified Stub)
    struct RayTracingShaderGroupDesc {
        // Type (General, TriangleHitGroup, ProceduralHitGroup)
        // Shader indices (gen, hit, miss, etc.)
    };
    struct RayTracingPipelineDesc {
        PipelineLayoutHandle layout;
        std::vector<ShaderStageDesc> shader_stages; // RayGen, Miss, Hit shaders
        std::vector<RayTracingShaderGroupDesc> shader_groups;
        Uint32 max_recursion_depth = 1;
        // Library info, pipeline cache, etc.
    };


    struct CommandBufferDesc {
        // QueueType queue_type = QueueType::Graphics; // Which queue family it belongs to
        Bool32 is_primary = true; // Or Secondary/Sub
        // Flags: OneTimeSubmit, SimultaneousUse etc.
    };

    struct SwapchainDesc {
        SurfaceHandle surface; // Added - Platform window surface
        Uint32 min_image_count = 2; // Request at least this many buffers
        Format format = Format::Undefined; // Desired surface format
        // ColorSpace color_space = ColorSpace::SRGB_NONLINEAR; // Desired color space
        Uint32 width = 0;
        Uint32 height = 0;
        Uint32 image_array_layers = 1;
        TextureUsageFlags image_usage = TextureUsageFlags::ColorAttachment; // How images will be used
        // SurfaceTransform transform = SurfaceTransform::Identity; // Pre-rotation
        // CompositeAlpha composite_alpha = CompositeAlpha::Opaque;
        PresentMode present_mode = PresentMode::Fifo;
        Bool32 clipped = true; // Allow clipping by other windows
        SwapchainHandle old_swapchain = {}; // For recreation optimization
    };

    // Describes one resource binding within a DescriptorSet
    struct DescriptorBindingInfo { // Renamed from DescriptorBinding
        Uint32 binding = 0;
        Uint32 array_element = 0; // Index within the binding array
        // Union or variant based on type? Or separate fields? Using separate for now.
        DescriptorType type = DescriptorType::UniformBuffer; // Keep type info here? Redundant with layout? Useful for validation.
        BufferHandle buffer = {};
        SizeType buffer_offset = 0;
        SizeType buffer_range = 0; // Use WHOLE_SIZE?
        TextureViewHandle texture_view = {}; // Use TextureView handle
        SamplerHandle sampler = {};
        ImageLayout image_layout = ImageLayout::ShaderReadOnlyOptimal; // Expected layout for image types
        AccelerationStructureHandle acceleration_structure = {}; // RT
    };

    // Framebuffer
    struct FramebufferAttachmentDesc { // Uses TextureView now
        TextureViewHandle view;
    };

    struct FramebufferDesc {
        RenderPassHandle render_pass; // Must be compatible
        std::vector<FramebufferAttachmentDesc> attachments; // Must match render pass attachments
        Uint32 width = 0;
        Uint32 height = 0;
        Uint32 layers = 1;
    };

    // Descriptor Set Update Structure
    struct WriteDescriptorSet { // Renamed from DescriptorUpdate
        DescriptorSetHandle dst_set;
        Uint32 dst_binding = 0;
        Uint32 dst_array_element = 0;
        Uint32 descriptor_count = 1; // How many descriptors to update (for arrays)
        DescriptorType descriptor_type = DescriptorType::UniformBuffer;
        // Use vectors for array updates? Simpler for now: one struct per update.
        // Provide pointer to info, or embed info? Embed for simplicity now.
        // Need to contain BufferInfo, ImageInfo, TexelBufferView etc.
        // Let's simplify: use the DescriptorBindingInfo struct
        std::vector<DescriptorBindingInfo> binding_infos; // Contains resource handles/ranges
    };

    struct CopyDescriptorSet {
        DescriptorSetHandle src_set;
        Uint32 src_binding = 0;
        Uint32 src_array_element = 0;
        DescriptorSetHandle dst_set;
        Uint32 dst_binding = 0;
        Uint32 dst_array_element = 0;
        Uint32 descriptor_count = 1;
    };


    // Viewport / Scissor
    struct Viewport {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
        float min_depth = 0.0f;
        float max_depth = 1.0f;
    };

    struct Rect2D {
        Int32 offset_x = 0;
        Int32 offset_y = 0;
        Uint32 extent_width = 0;
        Uint32 extent_height = 0;
    };


    // Clear Values
    struct ClearColorValue {
        float float32[4];
    }; // Or Uint32, Int32 variants
    struct ClearDepthStencilValue {
        float depth;
        Uint32 stencil;
    };
    using ClearValue = std::variant<ClearColorValue, ClearDepthStencilValue>; // Use variant

    struct RenderPassBeginInfo {
        RenderPassHandle render_pass;
        FramebufferHandle framebuffer;
        Rect2D render_area;
        std::vector<ClearValue> clear_values; // One per attachment with LoadOp::Clear
    };


    // Query Pool (Added)
    struct QueryPoolDesc {
        QueryType type = QueryType::Occlusion;
        Uint32 count = 1;
        // PipelineStatisticsFlags pipeline_statistics; // If type is PipelineStatistics
    };


    // Pipeline Cache (Added)
    struct PipelineCacheDesc {
        SizeType initial_data_size = 0;
        const void *initial_data = nullptr;
        // Flags?
    };


    // ----------------------------------------------------------
    // Synchronization Primitives & Barriers (Refined)
    // ----------------------------------------------------------

    // Global Memory Barrier (affects all resources)
    struct MemoryBarrier {
        PipelineStageFlags src_stage_mask = PipelineStageFlags::None;
        PipelineStageFlags dst_stage_mask = PipelineStageFlags::None;
        AccessFlags src_access_mask = AccessFlags::None;
        AccessFlags dst_access_mask = AccessFlags::None;
    };

    // Buffer-Specific Memory Barrier
    struct BufferMemoryBarrier {
        PipelineStageFlags src_stage_mask = PipelineStageFlags::None;
        PipelineStageFlags dst_stage_mask = PipelineStageFlags::None;
        AccessFlags src_access_mask = AccessFlags::None;
        AccessFlags dst_access_mask = AccessFlags::None;
        Uint32 src_queue_family_index = ~0U; // ~0U = Ignore
        Uint32 dst_queue_family_index = ~0U; // ~0U = Ignore
        BufferHandle buffer;
        SizeType offset = 0;
        SizeType size = ~0ULL; // WHOLE_SIZE
    };

    // Image-Specific Memory Barrier
    struct ImageMemoryBarrier {
        PipelineStageFlags src_stage_mask = PipelineStageFlags::None;
        PipelineStageFlags dst_stage_mask = PipelineStageFlags::None;
        AccessFlags src_access_mask = AccessFlags::None;
        AccessFlags dst_access_mask = AccessFlags::None;
        ImageLayout old_layout = ImageLayout::Undefined;
        ImageLayout new_layout = ImageLayout::Undefined;
        Uint32 src_queue_family_index = ~0U;
        Uint32 dst_queue_family_index = ~0U;
        TextureHandle image;
        // ImageSubresourceRange subresource_range; // Aspect(Color/Depth/Stencil), BaseMip, LevelCount, BaseLayer, LayerCount
        Uint32 base_mip_level = 0;
        Uint32 level_count = 1;
        Uint32 base_array_layer = 0;
        Uint32 layer_count = 1;
        // Aspect flags needed (Color, Depth, Stencil)
    };

    // Submit Info
    struct SubmitInfo {
        std::vector<SemaphoreHandle> wait_semaphores;
        std::vector<PipelineStageFlags> wait_dst_stage_masks; // One per wait semaphore
        std::vector<CommandBufferHandle> command_buffers;
        std::vector<SemaphoreHandle> signal_semaphores;
        FenceHandle signal_fence = {}; // Use handle now
    };

    // Present Info
    struct PresentInfo {
        std::vector<SemaphoreHandle> wait_semaphores;
        std::vector<SwapchainHandle> swapchains; // Can present multiple swapchains
        std::vector<Uint32> image_indices;   // One per swapchain
        // std::vector<Result> results; // Optional per-swapchain results
    };


    // ----------------------------------------------------------
    // Indirect Draw Command Structures
    // ----------------------------------------------------------
    struct DrawArraysIndirectCommand {
        Uint32 vertex_count;
        Uint32 instance_count;
        Uint32 first_vertex;
        Uint32 first_instance;
    };

    struct DrawElementsIndirectCommand { // Indexed version
        Uint32 index_count;
        Uint32 instance_count;
        Uint32 first_index;
        Int32 vertex_offset;
        Uint32 first_instance;
    };

    struct DrawMeshTasksIndirectCommand { // Mesh shader version
        Uint32 group_count_x;
        Uint32 group_count_y;
        Uint32 group_count_z;
    };

    struct DispatchIndirectCommand { // Compute version
        Uint32 group_count_x;
        Uint32 group_count_y;
        Uint32 group_count_z;
    };


    // ----------------------------------------------------------
    // Device Functions Table (Expanded and Refined)
    // ----------------------------------------------------------
    struct DeviceFunctions {
        // --- Core Lifecycle ---
        void (*shutdown_device)(void *ctx) = nullptr; // Added
        DeviceErrorType (*get_last_error)(void *ctx) = nullptr; // Renamed
        void (*clear_last_error)(void *ctx) = nullptr;

        Bool32 (*is_feature_supported)(void *ctx, DeviceFeature feature) = nullptr;
        // void (*get_device_properties)(void* ctx, DeviceProperties* props) = nullptr; // To query limits etc.
        // void (*get_queue)(void* ctx, QueueType type, Uint32 index, QueueHandle* queue) = nullptr;

        // --- Platform Surface Creation (Potentially outside core API) ---
        // SurfaceHandle (*create_surface_from_platform)(void* ctx, PlatformSurfaceInfo* info) = nullptr; // Added
        // void (*destroy_surface)(void* ctx, SurfaceHandle surface) = nullptr; // Added

        // --- Resource Creation/Destruction ---
        BufferHandle (*create_buffer)(void *ctx, const BufferDesc &desc) = nullptr;

        void (*destroy_buffer)(void *ctx, BufferHandle handle) = nullptr;

        TextureHandle (*create_texture)(void *ctx, const TextureDesc &desc) = nullptr;

        void (*destroy_texture)(void *ctx, TextureHandle handle) = nullptr;

        TextureViewHandle (*create_texture_view)(void *ctx, const TextureViewDesc &desc) = nullptr; // Added
        void (*destroy_texture_view)(void *ctx, TextureViewHandle handle) = nullptr; // Added
        SamplerHandle (*create_sampler)(void *ctx, const SamplerDesc &desc) = nullptr;

        void (*destroy_sampler)(void *ctx, SamplerHandle handle) = nullptr;

        ShaderHandle (*create_shader)(void *ctx, const ShaderDesc &desc) = nullptr;

        void (*destroy_shader)(void *ctx, ShaderHandle handle) = nullptr;

        PipelineLayoutHandle (*create_pipeline_layout)(void *ctx, const PipelineLayoutDesc &desc) = nullptr; // Added
        void (*destroy_pipeline_layout)(void *ctx, PipelineLayoutHandle handle) = nullptr; // Added
        PipelineHandle (*create_graphics_pipeline)(void *ctx, const PipelineDesc &desc,
                                                   PipelineCacheHandle cache) = nullptr; // Renamed, added cache
        PipelineHandle (*create_compute_pipeline)(void *ctx, const ComputePipelineDesc &desc,
                                                  PipelineCacheHandle cache) = nullptr; // Added cache
        PipelineHandle (*create_mesh_pipeline)(void *ctx, const MeshPipelineDesc &desc,
                                               PipelineCacheHandle cache) = nullptr; // Added cache
        // PipelineHandle (*create_ray_tracing_pipeline)(void* ctx, const RayTracingPipelineDesc& desc, PipelineCacheHandle cache) = nullptr; // Added (RT)
        void (*destroy_pipeline)(void *ctx, PipelineHandle handle) = nullptr; // Generic destroy
        PipelineCacheHandle (*create_pipeline_cache)(void *ctx, const PipelineCacheDesc &desc) = nullptr; // Added
        void (*destroy_pipeline_cache)(void *ctx, PipelineCacheHandle handle) = nullptr; // Added
        // Result (*get_pipeline_cache_data)(void* ctx, PipelineCacheHandle cache, size_t* size, void* data) = nullptr; // Added
        // Result (*merge_pipeline_caches)(void* ctx, PipelineCacheHandle dst, Uint32 src_count, const PipelineCacheHandle* srcs) = nullptr; // Added
        RenderPassHandle (*create_render_pass)(void *ctx, const RenderPassDesc &desc) = nullptr;

        void (*destroy_render_pass)(void *ctx, RenderPassHandle handle) = nullptr;

        FramebufferHandle (*create_framebuffer)(void *ctx, const FramebufferDesc &desc) = nullptr;

        void (*destroy_framebuffer)(void *ctx, FramebufferHandle handle) = nullptr;

        DescriptorSetLayoutHandle
        (*create_descriptor_set_layout)(void *ctx, const DescriptorSetLayoutDesc &desc) = nullptr;

        void (*destroy_descriptor_set_layout)(void *ctx, DescriptorSetLayoutHandle handle) = nullptr;

        // DescriptorPoolHandle (*create_descriptor_pool)(void* ctx, const DescriptorPoolDesc& desc) = nullptr; // Needed for set allocation
        // void (*destroy_descriptor_pool)(void* ctx, DescriptorPoolHandle pool) = nullptr;
        // Result (*allocate_descriptor_sets)(void* ctx, const DescriptorSetAllocateInfo& info, DescriptorSetHandle* sets) = nullptr; // Alloc from pool
        // void (*free_descriptor_sets)(void* ctx, DescriptorPoolHandle pool, Uint32 count, const DescriptorSetHandle* sets) = nullptr; // Free back to pool
        void
        (*update_descriptor_sets)(void *ctx, Uint32 write_count, const WriteDescriptorSet *writes, Uint32 copy_count,
                                  const CopyDescriptorSet *copies) = nullptr; // Use structs
        QueryPoolHandle (*create_query_pool)(void *ctx, const QueryPoolDesc &desc) = nullptr; // Added
        void (*destroy_query_pool)(void *ctx, QueryPoolHandle handle) = nullptr; // Added
        // AccelerationStructureHandle (*create_acceleration_structure)(...) = nullptr; // Added (RT)
        // void (*destroy_acceleration_structure)(...) = nullptr; // Added (RT)

        // --- Resource Info ---
        // void (*get_buffer_desc)(void* ctx, BufferHandle handle, BufferDesc* desc) = nullptr; // Added
        // void (*get_texture_desc)(void* ctx, TextureHandle handle, TextureDesc* desc) = nullptr; // Added
        // void (*get_buffer_memory_requirements)(void* ctx, BufferHandle handle, MemoryRequirements* reqs) = nullptr; // Added
        // void (*get_texture_memory_requirements)(void* ctx, TextureHandle handle, MemoryRequirements* reqs) = nullptr; // Added

        // --- Memory Mapping ---
        void *(*map_buffer)(void *ctx, BufferHandle buffer, SizeType offset, SizeType size,
                            AccessFlags /*MapFlags?*/) = nullptr;

        void (*unmap_buffer)(void *ctx, BufferHandle buffer) = nullptr;
        // void (*flush_mapped_buffer_ranges)(void* ctx, Uint32 range_count, const MappedMemoryRange* ranges) = nullptr;
        // void (*invalidate_mapped_buffer_ranges)(void* ctx, Uint32 range_count, const MappedMemoryRange* ranges) = nullptr;

        // --- Synchronization Objects ---
        FenceHandle (*create_fence)(void *ctx, Bool32 signaled) = nullptr; // Added signaled state
        void (*destroy_fence)(void *ctx, FenceHandle handle) = nullptr;

        void (*reset_fences)(void *ctx, Uint32 count, const FenceHandle *fences) = nullptr; // Use array
        Bool32 (*get_fence_status)(void *ctx, FenceHandle fence) = nullptr; // Added
        void (*wait_for_fences)(void *ctx, Uint32 count, const FenceHandle *fences, Bool32 wait_all,
                                Uint64 timeout_ns) = nullptr; // Use array
        SemaphoreHandle (*create_semaphore)(void *ctx /*, SemaphoreType type? Timeline?*/) = nullptr;

        void (*destroy_semaphore)(void *ctx, SemaphoreHandle handle) = nullptr;
        // Uint64 (*get_semaphore_counter_value)(...) // Timeline semaphores
        // void (*wait_semaphores)(...) // Timeline semaphores
        // void (*signal_semaphore)(...) // Timeline semaphores

        // --- Swapchain ---
        SwapchainHandle (*create_swapchain)(void *ctx, const SwapchainDesc &desc) = nullptr;

        void (*destroy_swapchain)(void *ctx, SwapchainHandle handle) = nullptr;

        Bool32
        (*acquire_next_image)(void *ctx, SwapchainHandle swapchain, Uint64 timeout_ns, SemaphoreHandle signal_semaphore,
                              FenceHandle signal_fence, Uint32 *image_index) = nullptr; // Simplified result
        void
        (*queue_present)(void *ctx /*QueueHandle queue*/, const PresentInfo &info) = nullptr; // Renamed, needs queue

        // --- Command Pool & Buffer ---
        // CommandPoolHandle (*create_command_pool)(void* ctx, const CommandPoolDesc& desc) = nullptr;
        // void (*destroy_command_pool)(void* ctx, CommandPoolHandle pool) = nullptr;
        // void (*reset_command_pool)(void* ctx, CommandPoolHandle pool) = nullptr;
        CommandBufferHandle
        (*allocate_command_buffer)(void *ctx /*CommandPool pool*/, const CommandBufferDesc &desc) = nullptr; // Renamed
        void (*free_command_buffer)(void *ctx /*CommandPool pool*/, CommandBufferHandle handle) = nullptr; // Added
        void
        (*begin_command_buffer)(void *ctx, CommandBufferHandle cmdbuf /*, CommandBufferBeginInfo* info?*/) = nullptr;

        void (*end_command_buffer)(void *ctx, CommandBufferHandle cmdbuf) = nullptr;

        void (*reset_command_buffer)(void *ctx, CommandBufferHandle cmdbuf /*, ResetFlags?*/) = nullptr; // Added

        // --- Queue Submission ---
        void (*queue_submit)(void *ctx, /*QueueHandle queue,*/ Uint32 submit_count,
                             const SubmitInfo *submits) = nullptr; // Renamed, needs queue
        // void (*queue_wait_idle)(void* ctx, QueueHandle queue) = nullptr;
        // void (*device_wait_idle)(void* ctx) = nullptr;

        // --- Command Buffer Recording Commands ---
        // ** State Binding **
        void (*cmd_bind_pipeline)(void *ctx, CommandBufferHandle cmdbuf,
                                  PipelineHandle pipeline /*PipelineBindPoint type*/) = nullptr;

        void (*cmd_bind_descriptor_sets)(void *ctx, CommandBufferHandle cmdbuf, PipelineLayoutHandle layout,
                                         Uint32 first_set, Uint32 count, const DescriptorSetHandle *sets,
                                         Uint32 dynamic_offset_count, const Uint32 *dynamic_offsets) = nullptr; // Added
        void (*cmd_bind_vertex_buffers)(void *ctx, CommandBufferHandle cmdbuf, Uint32 first_binding, Uint32 count,
                                        const BufferHandle *buffers,
                                        const SizeType *offsets) = nullptr; // SizeType offsets
        void (*cmd_bind_index_buffer)(void *ctx, CommandBufferHandle cmdbuf, BufferHandle buffer, SizeType offset,
                                      IndexType index_type) = nullptr; // SizeType offset

        // ** Dynamic State **
        void (*cmd_set_viewport)(void *ctx, CommandBufferHandle cmdbuf, Uint32 first_viewport, Uint32 count,
                                 const Viewport *viewports) = nullptr; // Allow multiple
        void (*cmd_set_scissor)(void *ctx, CommandBufferHandle cmdbuf, Uint32 first_scissor, Uint32 count,
                                const Rect2D *scissors) = nullptr; // Allow multiple
        void (*cmd_set_line_width)(void *ctx, CommandBufferHandle cmdbuf, float line_width) = nullptr; // Added
        void (*cmd_set_depth_bias)(void *ctx, CommandBufferHandle cmdbuf, float constant_factor, float clamp,
                                   float slope_factor) = nullptr; // Added
        void
        (*cmd_set_blend_constants)(void *ctx, CommandBufferHandle cmdbuf, const float constants[4]) = nullptr; // Added
        void (*cmd_set_depth_bounds)(void *ctx, CommandBufferHandle cmdbuf, float min_bounds,
                                     float max_bounds) = nullptr; // Added
        void (*cmd_set_stencil_compare_mask)(void *ctx, CommandBufferHandle cmdbuf /*StencilFaceFlags faces*/,
                                             Uint32 mask) = nullptr; // Added
        void (*cmd_set_stencil_write_mask)(void *ctx, CommandBufferHandle cmdbuf /*StencilFaceFlags faces*/,
                                           Uint32 mask) = nullptr; // Added
        void (*cmd_set_stencil_reference)(void *ctx, CommandBufferHandle cmdbuf /*StencilFaceFlags faces*/,
                                          Uint32 reference) = nullptr; // Added

        // ** Push Constants **
        void (*cmd_update_push_constants)(void *ctx, CommandBufferHandle cmdbuf, PipelineLayoutHandle layout,
                                          ShaderStageFlags stage_flags, Uint32 offset, Uint32 size,
                                          const void *data) = nullptr; // Use stage flags

        // ** Drawing **
        void (*cmd_draw)(void *ctx, CommandBufferHandle cmdbuf, Uint32 vertex_count, Uint32 instance_count,
                         Uint32 first_vertex, Uint32 first_instance) = nullptr;

        void (*cmd_draw_indexed)(void *ctx, CommandBufferHandle cmdbuf, Uint32 index_count, Uint32 instance_count,
                                 Uint32 first_index, Int32 vertex_offset, Uint32 first_instance) = nullptr;

        void (*cmd_draw_indirect)(void *ctx, CommandBufferHandle cmdbuf, BufferHandle buffer, SizeType offset,
                                  Uint32 draw_count, Uint32 stride) = nullptr; // Added
        void (*cmd_draw_indexed_indirect)(void *ctx, CommandBufferHandle cmdbuf, BufferHandle buffer, SizeType offset,
                                          Uint32 draw_count, Uint32 stride) = nullptr; // Added
        // void (*cmd_draw_indirect_count)(...) // Added (Variable draw count)
        // void (*cmd_draw_indexed_indirect_count)(...) // Added (Variable draw count)
        void (*cmd_draw_mesh_tasks)(void *ctx, CommandBufferHandle cmdbuf, Uint32 group_count_x, Uint32 group_count_y,
                                    Uint32 group_count_z) = nullptr; // Added (Mesh)
        void
        (*cmd_draw_mesh_tasks_indirect)(void *ctx, CommandBufferHandle cmdbuf, BufferHandle buffer, SizeType offset,
                                        Uint32 draw_count, Uint32 stride) = nullptr; // Added (Mesh)
        // void (*cmd_draw_mesh_tasks_indirect_count)(...) // Added (Mesh)

        // ** Compute **
        void (*cmd_dispatch)(void *ctx, CommandBufferHandle cmdbuf, Uint32 group_count_x, Uint32 group_count_y,
                             Uint32 group_count_z) = nullptr;

        void (*cmd_dispatch_indirect)(void *ctx, CommandBufferHandle cmdbuf, BufferHandle buffer,
                                      SizeType offset) = nullptr; // Added

        // ** Render Pass **
        void (*cmd_begin_render_pass)(void *ctx, CommandBufferHandle cmdbuf,
                                      const RenderPassBeginInfo &begin_info /*SubpassContents contents*/) = nullptr;

        void (*cmd_next_subpass)(void *ctx, CommandBufferHandle cmdbuf /*SubpassContents contents*/) = nullptr; // Added
        void (*cmd_end_render_pass)(void *ctx, CommandBufferHandle cmdbuf) = nullptr;
        // void (*cmd_execute_commands)(void* ctx, CommandBufferHandle primary_cmdbuf, Uint32 count, const CommandBufferHandle* secondary_cmdbufs); // Secondary command buffers

        // ** Synchronization **
        void (*cmd_pipeline_barrier)(void *ctx, CommandBufferHandle cmdbuf, PipelineStageFlags src_stage_mask,
                                     PipelineStageFlags dst_stage_mask /*DependencyFlags flags*/,
                                     Uint32 memory_barrier_count, const MemoryBarrier *memory_barriers,
                                     Uint32 buffer_barrier_count, const BufferMemoryBarrier *buffer_barriers,
                                     Uint32 image_barrier_count,
                                     const ImageMemoryBarrier *image_barriers) = nullptr; // Added granular barriers
        // void (*cmd_wait_events)(...)
        // void (*cmd_set_event)(...)
        // void (*cmd_reset_event)(...)

        // ** Transfer / Copy **
        void (*cmd_copy_buffer)(void *ctx, CommandBufferHandle cmdbuf, BufferHandle src, BufferHandle dst,
                                Uint32 region_count,
                                const /*BufferCopyRegion**/ void *regions) = nullptr; // Use regions
        void (*cmd_copy_texture)(void *ctx, CommandBufferHandle cmdbuf, TextureHandle src, ImageLayout src_layout,
                                 TextureHandle dst, ImageLayout dst_layout, Uint32 region_count,
                                 const /*ImageCopyRegion**/ void *regions) = nullptr; // Use regions, layouts
        void (*cmd_copy_buffer_to_texture)(void *ctx, CommandBufferHandle cmdbuf, BufferHandle src, TextureHandle dst,
                                           ImageLayout dst_layout, Uint32 region_count,
                                           const /*BufferImageCopyRegion**/ void *regions) = nullptr; // Added
        void
        (*cmd_copy_texture_to_buffer)(void *ctx, CommandBufferHandle cmdbuf, TextureHandle src, ImageLayout src_layout,
                                      BufferHandle dst, Uint32 region_count,
                                      const /*BufferImageCopyRegion**/ void *regions) = nullptr; // Added
        void (*cmd_blit_texture)(void *ctx, CommandBufferHandle cmdbuf, TextureHandle src, ImageLayout src_layout,
                                 TextureHandle dst, ImageLayout dst_layout, Uint32 region_count,
                                 const /*ImageBlitRegion**/ void *regions, Filter filter) = nullptr; // Added
        void (*cmd_resolve_texture)(void *ctx, CommandBufferHandle cmdbuf, TextureHandle src, ImageLayout src_layout,
                                    TextureHandle dst, ImageLayout dst_layout, Uint32 region_count,
                                    const /*ImageResolveRegion**/ void *regions) = nullptr; // Added
        void
        (*cmd_update_buffer)(void *ctx, CommandBufferHandle cmdbuf, BufferHandle dst, SizeType offset, SizeType size,
                             const void *data) = nullptr; // Direct update (small data)
        void (*cmd_fill_buffer)(void *ctx, CommandBufferHandle cmdbuf, BufferHandle dst, SizeType offset, SizeType size,
                                Uint32 data) = nullptr; // Added
        void (*cmd_clear_color_texture)(void *ctx, CommandBufferHandle cmdbuf, TextureHandle image, ImageLayout layout,
                                        const ClearColorValue *color, Uint32 range_count,
                                        const /*ImageSubresourceRange**/ void *ranges) = nullptr; // Added
        void (*cmd_clear_depth_stencil_texture)(void *ctx, CommandBufferHandle cmdbuf, TextureHandle image,
                                                ImageLayout layout, const ClearDepthStencilValue *value,
                                                Uint32 range_count,
                                                const /*ImageSubresourceRange**/ void *ranges) = nullptr; // Added
        // void (*cmd_clear_attachments)(...) // Clear currently bound attachments

        // ** Query **
        void (*cmd_reset_query_pool)(void *ctx, CommandBufferHandle cmdbuf, QueryPoolHandle pool, Uint32 first_query,
                                     Uint32 query_count) = nullptr; // Can be outside render pass now
        void (*cmd_begin_query)(void *ctx, CommandBufferHandle cmdbuf, QueryPoolHandle pool,
                                Uint32 query /*QueryControlFlags flags*/) = nullptr;

        void (*cmd_end_query)(void *ctx, CommandBufferHandle cmdbuf, QueryPoolHandle pool, Uint32 query) = nullptr;

        void
        (*cmd_write_timestamp)(void *ctx, CommandBufferHandle cmdbuf, PipelineStageFlags stage, QueryPoolHandle pool,
                               Uint32 query) = nullptr; // Added
        void
        (*cmd_copy_query_pool_results)(void *ctx, CommandBufferHandle cmdbuf, QueryPoolHandle pool, Uint32 first_query,
                                       Uint32 query_count, BufferHandle dst_buffer, SizeType dst_offset,
                                       SizeType stride /*QueryResultFlags flags*/) = nullptr; // GPU copy
        void (*get_query_pool_results)(void *ctx /*Device or Queue?*/, QueryPoolHandle pool, Uint32 first_query,
                                       Uint32 query_count, SizeType data_size, void *data,
                                       SizeType stride /*QueryResultFlags flags*/) = nullptr; // CPU copy

        // ** Debug Utils **
        void (*cmd_begin_debug_marker)(void *ctx, CommandBufferHandle cmdbuf, const char *marker_name,
                                       const float color[4]) = nullptr;

        void (*cmd_end_debug_marker)(void *ctx, CommandBufferHandle cmdbuf) = nullptr;

        void (*cmd_insert_debug_marker)(void *ctx, CommandBufferHandle cmdbuf, const char *marker_name,
                                        const float color[4]) = nullptr;
        // void (*set_object_name)(void* ctx, ObjectType type, Uint64 handle_id, const char* name) = nullptr;

        // ** Ray Tracing Commands (Stubs) **
        // void (*cmd_build_acceleration_structures)(...) = nullptr; // Added (RT)
        // void (*cmd_copy_acceleration_structure)(...) = nullptr; // Added (RT)
        // void (*cmd_write_acceleration_structures_properties)(...) = nullptr; // Added (RT)
        // void (*cmd_trace_rays)(...) = nullptr; // Added (RT)
        // void (*cmd_trace_rays_indirect)(...) = nullptr; // Added (RT)
        // void (*get_acceleration_structure_build_sizes)(...) = nullptr; // Added (RT)
        // void (*get_ray_tracing_shader_group_handles)(...) = nullptr; // Added (RT)

        // --- Context Pointer ---
        void *backend_context = nullptr; // Make context part of the table itself? Or passed separately? Passed separately in Device class.
    };


    // ----------------------------------------------------------
    // Device Class (C++ Wrapper)
    // ----------------------------------------------------------
    class Device {
    public:
        Device(const DeviceFunctions &funcs, void *backend_context, BackendType backend_type)
                : funcs_(funcs), backend_context_(backend_context), backend_type_(backend_type) {}

        // Prevent copying/moving if it holds backend state implicitly
        Device(const Device &) = delete;

        Device(Device &&) = delete;

        Device &operator=(const Device &) = delete;

        Device &operator=(Device &&) = delete;

        virtual ~Device() {
            // Note: Shutdown should ideally be called explicitly before destruction.
            // if (funcs_.shutdown_device) {
            //     funcs_.shutdown_device(backend_context_);
            // }
        }

        // --- Core Lifecycle ---
        void shutdown_device() { if (funcs_.shutdown_device) funcs_.shutdown_device(backend_context_); }

        [[nodiscard]] DeviceErrorType get_last_error() const {
            return funcs_.get_last_error ? funcs_.get_last_error(backend_context_) : DeviceErrorType::Unknown;
        }

        void clear_last_error() { if (funcs_.clear_last_error) funcs_.clear_last_error(backend_context_); }

        [[nodiscard]] bool is_feature_supported(DeviceFeature feature) const {
            return funcs_.is_feature_supported ? funcs_.is_feature_supported(backend_context_, feature) : false;
        }

        [[nodiscard]] BackendType get_backend_type() const { return backend_type_; }

        [[nodiscard]] void *get_backend_context() const { return backend_context_; }

        // --- Resource Creation/Destruction (Wrappers) ---
        // Example for Buffer:
        [[nodiscard]] BufferHandle create_buffer(const BufferDesc &desc) {
            return funcs_.create_buffer ? funcs_.create_buffer(backend_context_, desc) : BufferHandle{};
        }

        void destroy_buffer(BufferHandle handle) {
            if (is_valid(handle) && funcs_.destroy_buffer)
                funcs_.destroy_buffer(backend_context_, handle);
        }

        [[nodiscard]] TextureHandle create_texture(const TextureDesc &desc) {
            return funcs_.create_texture ? funcs_.create_texture(backend_context_, desc) : TextureHandle{};
        }

        void destroy_texture(TextureHandle handle) {
            if (is_valid(handle) && funcs_.destroy_texture)
                funcs_.destroy_texture(backend_context_, handle);
        }

        [[nodiscard]] TextureViewHandle create_texture_view(const TextureViewDesc &desc) {
            return funcs_.create_texture_view ? funcs_.create_texture_view(backend_context_, desc)
                                              : TextureViewHandle{};
        }

        void destroy_texture_view(TextureViewHandle handle) {
            if (is_valid(handle) && funcs_.destroy_texture_view)funcs_.destroy_texture_view(backend_context_, handle);
        }

        [[nodiscard]] SamplerHandle create_sampler(const SamplerDesc &desc) {
            return funcs_.create_sampler ? funcs_.create_sampler(backend_context_, desc) : SamplerHandle{};
        }

        void destroy_sampler(SamplerHandle handle) {
            if (is_valid(handle) && funcs_.destroy_sampler)
                funcs_.destroy_sampler(backend_context_, handle);
        }

        [[nodiscard]] ShaderHandle create_shader(const ShaderDesc &desc) {
            return funcs_.create_shader ? funcs_.create_shader(backend_context_, desc) : ShaderHandle{};
        }

        void destroy_shader(ShaderHandle handle) {
            if (is_valid(handle) && funcs_.destroy_shader)
                funcs_.destroy_shader(backend_context_, handle);
        }

        [[nodiscard]] PipelineLayoutHandle create_pipeline_layout(const PipelineLayoutDesc &desc) {
            return funcs_.create_pipeline_layout ? funcs_.create_pipeline_layout(backend_context_, desc)
                                                 : PipelineLayoutHandle{};
        }

        void destroy_pipeline_layout(PipelineLayoutHandle handle) {
            if (is_valid(handle) && funcs_.destroy_pipeline_layout)
                funcs_.destroy_pipeline_layout(backend_context_, handle);
        }

        [[nodiscard]] PipelineHandle create_graphics_pipeline(const PipelineDesc &desc,
                                                              PipelineCacheHandle cache = {}) {
            return funcs_.create_graphics_pipeline ? funcs_.create_graphics_pipeline(backend_context_, desc, cache)
                                                   : PipelineHandle{};
        }

        [[nodiscard]] PipelineHandle create_compute_pipeline(const ComputePipelineDesc &desc,
                                                             PipelineCacheHandle cache = {}) {
            return funcs_.create_compute_pipeline ? funcs_.create_compute_pipeline(backend_context_, desc, cache)
                                                  : PipelineHandle{};
        }

        [[nodiscard]] PipelineHandle create_mesh_pipeline(const MeshPipelineDesc &desc,
                                                          PipelineCacheHandle cache = {}) {
            return funcs_.create_mesh_pipeline ? funcs_.create_mesh_pipeline(backend_context_, desc, cache)
                                               : PipelineHandle{};
        }

        void destroy_pipeline(PipelineHandle handle) {
            if (is_valid(handle) && funcs_.destroy_pipeline)
                funcs_.destroy_pipeline(backend_context_, handle);
        }

        [[nodiscard]] PipelineCacheHandle create_pipeline_cache(const PipelineCacheDesc &desc = {}) {
            return funcs_.create_pipeline_cache ? funcs_.create_pipeline_cache(backend_context_, desc)
                                                : PipelineCacheHandle{};
        }

        void destroy_pipeline_cache(PipelineCacheHandle handle) {
            if (is_valid(handle) && funcs_.destroy_pipeline_cache)
                funcs_.destroy_pipeline_cache(backend_context_, handle);
        }

        [[nodiscard]] RenderPassHandle create_render_pass(const RenderPassDesc &desc) {
            return funcs_.create_render_pass ? funcs_.create_render_pass(backend_context_, desc) : RenderPassHandle{};
        }

        void destroy_render_pass(RenderPassHandle handle) {
            if (is_valid(handle) && funcs_.destroy_render_pass)
                funcs_.destroy_render_pass(backend_context_, handle);
        }

        [[nodiscard]] FramebufferHandle create_framebuffer(const FramebufferDesc &desc) {
            return funcs_.create_framebuffer ? funcs_.create_framebuffer(backend_context_, desc) : FramebufferHandle{};
        }

        void destroy_framebuffer(FramebufferHandle handle) {
            if (is_valid(handle) && funcs_.destroy_framebuffer)
                funcs_.destroy_framebuffer(backend_context_, handle);
        }

        [[nodiscard]] DescriptorSetLayoutHandle create_descriptor_set_layout(const DescriptorSetLayoutDesc &desc) {
            return funcs_.create_descriptor_set_layout ? funcs_.create_descriptor_set_layout(backend_context_, desc)
                                                       : DescriptorSetLayoutHandle{};
        }

        void destroy_descriptor_set_layout(DescriptorSetLayoutHandle handle) {
            if (is_valid(handle) && funcs_.destroy_descriptor_set_layout)
                funcs_.destroy_descriptor_set_layout(backend_context_, handle);
        }

        // Note: Descriptor Set allocation/freeing requires a Descriptor Pool concept, omitted here for brevity but essential
        // DescriptorSetHandle allocate_descriptor_set(...)
        // void free_descriptor_set(...)
        void update_descriptor_sets(const std::vector<WriteDescriptorSet> &writes,
                                    const std::vector<CopyDescriptorSet> &copies = {}) {
            if (funcs_.update_descriptor_sets)
                funcs_.update_descriptor_sets(backend_context_, static_cast<Uint32>(writes.size()), writes.data(),
                                              static_cast<Uint32>(copies.size()), copies.data());
        }

        [[nodiscard]] QueryPoolHandle create_query_pool(const QueryPoolDesc &desc) {
            return funcs_.create_query_pool ? funcs_.create_query_pool(backend_context_, desc) : QueryPoolHandle{};
        }

        void destroy_query_pool(QueryPoolHandle handle) {
            if (is_valid(handle) && funcs_.destroy_query_pool)
                funcs_.destroy_query_pool(backend_context_, handle);
        }


        // --- Memory Mapping ---
        [[nodiscard]] void *map_buffer(BufferHandle buffer, SizeType offset = 0,
                                       SizeType size = ~0ULL /* WHOLE_SIZE */) {
            return funcs_.map_buffer ? funcs_.map_buffer(backend_context_, buffer, offset, size, AccessFlags::None)
                                     : nullptr;
        } // MapFlags?
        void unmap_buffer(BufferHandle buffer) {
            if (is_valid(buffer) && funcs_.unmap_buffer)
                funcs_.unmap_buffer(backend_context_, buffer);
        }

        // --- Synchronization Objects ---
        [[nodiscard]] FenceHandle create_fence(bool signaled = false) {
            return funcs_.create_fence ? funcs_.create_fence(backend_context_, signaled) : FenceHandle{};
        }

        void destroy_fence(FenceHandle handle) {
            if (is_valid(handle) && funcs_.destroy_fence)
                funcs_.destroy_fence(backend_context_, handle);
        }

        void reset_fences(const std::vector<FenceHandle> &fences) {
            if (!fences.empty() && funcs_.reset_fences)
                funcs_.reset_fences(backend_context_, static_cast<Uint32>(fences.size()), fences.data());
        }

        [[nodiscard]] bool get_fence_status(FenceHandle fence) {
            return funcs_.get_fence_status ? funcs_.get_fence_status(backend_context_, fence) : false;
        }

        void wait_for_fences(const std::vector<FenceHandle> &fences, bool wait_all, Uint64 timeout_ns) {
            if (!fences.empty() && funcs_.wait_for_fences)
                funcs_.wait_for_fences(backend_context_, static_cast<Uint32>(fences.size()), fences.data(), wait_all,
                                       timeout_ns);
        }

        [[nodiscard]] SemaphoreHandle create_semaphore() {
            return funcs_.create_semaphore ? funcs_.create_semaphore(backend_context_) : SemaphoreHandle{};
        }

        void destroy_semaphore(SemaphoreHandle handle) {
            if (is_valid(handle) && funcs_.destroy_semaphore)
                funcs_.destroy_semaphore(backend_context_, handle);
        }

        // --- Swapchain ---
        [[nodiscard]] SwapchainHandle create_swapchain(const SwapchainDesc &desc) {
            return funcs_.create_swapchain ? funcs_.create_swapchain(backend_context_, desc) : SwapchainHandle{};
        }

        void destroy_swapchain(SwapchainHandle handle) {
            if (is_valid(handle) && funcs_.destroy_swapchain)
                funcs_.destroy_swapchain(backend_context_, handle);
        }

        [[nodiscard]] bool
        acquire_next_image(SwapchainHandle swapchain, Uint64 timeout_ns, SemaphoreHandle signal_semaphore,
                           FenceHandle signal_fence, Uint32 &out_image_index) {
            return funcs_.acquire_next_image ? funcs_.acquire_next_image(backend_context_, swapchain, timeout_ns,
                                                                         signal_semaphore, signal_fence,
                                                                         &out_image_index) : false;
        }
        // queue_present needs QueueHandle
         void queue_present(/*QueueHandle queue,*/ const PresentInfo& info) {
            if (funcs_.queue_present)
                funcs_.queue_present(backend_context_, info);
        }


        // --- Command Pool & Buffer ---
        // Need Command Pool handle and functions
        [[nodiscard]] CommandBufferHandle allocate_command_buffer(/*CommandPool pool,*/
                const CommandBufferDesc &desc = {true}) {
            return funcs_.allocate_command_buffer ? funcs_.allocate_command_buffer(backend_context_, desc)
                                                  : CommandBufferHandle{};
        }

        void free_command_buffer(/*CommandPool pool,*/ CommandBufferHandle handle) {
            if (is_valid(handle) && funcs_.free_command_buffer)funcs_.free_command_buffer(backend_context_, handle);
        }

        void begin_command_buffer(CommandBufferHandle cmdbuf) {
            if (funcs_.begin_command_buffer)
                funcs_.begin_command_buffer(backend_context_, cmdbuf);
        }

        void end_command_buffer(CommandBufferHandle cmdbuf) {
            if (funcs_.end_command_buffer)
                funcs_.end_command_buffer(backend_context_, cmdbuf);
        }

        void reset_command_buffer(CommandBufferHandle cmdbuf) {
            if (funcs_.reset_command_buffer)
                funcs_.reset_command_buffer(backend_context_, cmdbuf);
        }

        // --- Queue Submission ---
        // Need Queue handle and functions
        void queue_submit(/*QueueHandle queue,*/ const std::vector<SubmitInfo>& submits) {
            if (funcs_.queue_submit)
                funcs_.queue_submit(backend_context_, static_cast<Uint32>(submits.size()), submits.data());
        }


        // --- Public member access to functions (alternative to wrapper methods) ---
        // const DeviceFunctions& get_functions() const { return funcs_; }
        // void* get_context() const { return backend_context_; }

        DeviceFunctions funcs_;

    protected: // Make protected or private if Device class manages lifetime/state
        void *backend_context_ = nullptr;
        BackendType backend_type_;
    };


    // ==========================================================
    // Standalone Command Functions (for use inside command buffer recording)
    // ==========================================================
    // These functions need the Device AND the CommandBufferHandle passed to them.
    // They act as wrappers around the function table calls, providing a slightly
    // cleaner C++ interface than calling funcs_.cmd_whatever(...) directly.

    inline void cmd_bind_pipeline(Device *device, CommandBufferHandle cmdbuf, PipelineHandle pipeline) {
        if (device && is_valid(cmdbuf) && device->funcs_.cmd_bind_pipeline) {
            device->funcs_.cmd_bind_pipeline(device->get_backend_context(), cmdbuf, pipeline);
        }
    }

    inline void
    cmd_bind_descriptor_sets(Device *device, CommandBufferHandle cmdbuf, PipelineLayoutHandle layout, Uint32 first_set,
                             const std::vector<DescriptorSetHandle> &sets,
                             const std::vector<Uint32> &dynamic_offsets = {}) {
        if (device && is_valid(cmdbuf) && device->funcs_.cmd_bind_descriptor_sets) {
            device->funcs_.cmd_bind_descriptor_sets(device->get_backend_context(), cmdbuf, layout, first_set,
                                                    static_cast<Uint32>(sets.size()), sets.data(),
                                                    static_cast<Uint32>(dynamic_offsets.size()),
                                                    dynamic_offsets.data());
        }
    }

    inline void cmd_bind_vertex_buffers(Device *device, CommandBufferHandle cmdbuf, Uint32 first_binding,
                                        const std::vector<BufferHandle> &buffers,
                                        const std::vector<SizeType> &offsets) {
        // Ensure buffers and offsets have the same size
        if (device && is_valid(cmdbuf) && !buffers.empty() && buffers.size() == offsets.size() &&
            device->funcs_.cmd_bind_vertex_buffers) {
            device->funcs_.cmd_bind_vertex_buffers(device->get_backend_context(), cmdbuf, first_binding,
                                                   static_cast<Uint32>(buffers.size()), buffers.data(), offsets.data());
        }
    }

    inline void cmd_bind_index_buffer(Device *device, CommandBufferHandle cmdbuf, BufferHandle buffer, SizeType offset,
                                      IndexType index_type) {
        if (device && is_valid(cmdbuf) && device->funcs_.cmd_bind_index_buffer) {
            device->funcs_.cmd_bind_index_buffer(device->get_backend_context(), cmdbuf, buffer, offset, index_type);
        }
    }

    // ... (Wrap other cmd_* functions similarly) ...

    inline void
    cmd_set_viewport(Device *device, CommandBufferHandle cmdbuf, const Viewport &viewport, Uint32 first_viewport = 0) {
        if (device && is_valid(cmdbuf) && device->funcs_.cmd_set_viewport) {
            device->funcs_.cmd_set_viewport(device->get_backend_context(), cmdbuf, first_viewport, 1, &viewport);
        }
    }

    inline void
    cmd_set_scissor(Device *device, CommandBufferHandle cmdbuf, const Rect2D &rect, Uint32 first_scissor = 0) {
        if (device && is_valid(cmdbuf) && device->funcs_.cmd_set_scissor) {
            device->funcs_.cmd_set_scissor(device->get_backend_context(), cmdbuf, first_scissor, 1, &rect);
        }
    }

    inline void cmd_update_push_constants(Device *device, CommandBufferHandle cmdbuf, PipelineLayoutHandle layout,
                                          ShaderStageFlags stage_flags, Uint32 offset, Uint32 size, const void *data) {
        if (device && is_valid(cmdbuf) && data && device->funcs_.cmd_update_push_constants) {
            device->funcs_.cmd_update_push_constants(device->get_backend_context(), cmdbuf, layout, stage_flags, offset,
                                                     size, data);
        }
    }

    inline void cmd_draw(Device *device, CommandBufferHandle cmdbuf, Uint32 vertex_count, Uint32 instance_count,
                         Uint32 first_vertex, Uint32 first_instance) {
        if (device && is_valid(cmdbuf) && device->funcs_.cmd_draw) {
            device->funcs_.cmd_draw(device->get_backend_context(), cmdbuf, vertex_count, instance_count, first_vertex,
                                    first_instance);
        }
    }

    inline void cmd_draw_indexed(Device *device, CommandBufferHandle cmdbuf, Uint32 index_count, Uint32 instance_count,
                                 Uint32 first_index, Int32 vertex_offset, Uint32 first_instance) {
        if (device && is_valid(cmdbuf) && device->funcs_.cmd_draw_indexed) {
            device->funcs_.cmd_draw_indexed(device->get_backend_context(), cmdbuf, index_count, instance_count,
                                            first_index, vertex_offset, first_instance);
        }
    }

    inline void
    cmd_begin_render_pass(Device *device, CommandBufferHandle cmdbuf, const RenderPassBeginInfo &begin_info) {
        if (device && is_valid(cmdbuf) && device->funcs_.cmd_begin_render_pass) {
            device->funcs_.cmd_begin_render_pass(device->get_backend_context(), cmdbuf, begin_info);
        }
    }

    inline void cmd_end_render_pass(Device *device, CommandBufferHandle cmdbuf) {
        if (device && is_valid(cmdbuf) && device->funcs_.cmd_end_render_pass) {
            device->funcs_.cmd_end_render_pass(device->get_backend_context(), cmdbuf);
        }
    }

    // ... etc for all cmd_ functions ...

} // namespace Bcg::Graphics