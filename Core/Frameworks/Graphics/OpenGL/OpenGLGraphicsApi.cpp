#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include "GraphicsApi.h"  // Your abstract API header
#include <glad/gl.h>      // Using glad as the GL loader

namespace Bcg::Graphics {

// Extended backend context to track more GL objects.
struct GLDeviceContext {
    // Buffers
    Uint32 next_buffer_id = 1;
    std::unordered_map<Uint32, GLuint> buffers;

    // Shaders
    Uint32 next_shader_id = 1;
    std::unordered_map<Uint32, GLuint> shaders;

    // Pipelines (linked programs)
    Uint32 next_pipeline_id = 1;
    std::unordered_map<Uint32, GLuint> pipelines;

    // Textures
    Uint32 next_texture_id = 1;
    std::unordered_map<Uint32, GLuint> textures;

    // Samplers
    Uint32 next_sampler_id = 1;
    std::unordered_map<Uint32, GLuint> samplers;

    // Render passes (dummy, since OpenGL doesn’t have them)
    Uint32 next_renderpass_id = 1;
    std::unordered_map<Uint32, int> renderpasses;

    // Framebuffers
    Uint32 next_framebuffer_id = 1;
    std::unordered_map<Uint32, GLuint> framebuffers;

    // Descriptor Set Layouts (dummy)
    Uint32 next_descriptor_set_layout_id = 1;
    std::unordered_map<Uint32, int> descriptor_set_layouts;

    // Descriptor Sets (dummy)
    Uint32 next_descriptor_set_id = 1;
    std::unordered_map<Uint32, int> descriptor_sets;

    // Command Buffers (dummy, immediate mode)
    Uint32 next_command_buffer_id = 1;
    std::unordered_map<Uint32, int> command_buffers;

    // Fences (using GLsync)
    Uint32 next_fence_id = 1;
    std::unordered_map<Uint32, GLsync> fences;

    // Semaphores (dummy)
    Uint32 next_semaphore_id = 1;
    std::unordered_map<Uint32, int> semaphores;

    // Swapchains (dummy)
    Uint32 next_swapchain_id = 1;
    std::unordered_map<Uint32, int> swapchains;
};

// -------------------------
// Buffer Functions
// -------------------------
static GLenum GLUsageFromBufferUsage(BufferUsage usage) {
    switch (usage) {
        case BufferUsage::VertexBuffer: return GL_ARRAY_BUFFER;
        case BufferUsage::IndexBuffer:  return GL_ELEMENT_ARRAY_BUFFER;
        case BufferUsage::UniformBuffer:return GL_UNIFORM_BUFFER;
        case BufferUsage::StorageBuffer:return GL_SHADER_STORAGE_BUFFER;
        default:                        return GL_ARRAY_BUFFER;
    }
}

static BufferHandle GL_CreateBuffer(void *ctx, const BufferDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    GLuint gl_buffer = 0;
    glCreateBuffers(1, &gl_buffer);
    // Allocate storage (using dynamic draw for simplicity)
    glNamedBufferData(gl_buffer, desc.size, nullptr, GL_DYNAMIC_DRAW);

    BufferHandle handle;
    handle.id = gl_ctx->next_buffer_id++;
    gl_ctx->buffers[handle.id] = gl_buffer;
    return handle;
}

static void GL_DestroyBuffer(void *ctx, BufferHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    auto it = gl_ctx->buffers.find(handle.id);
    if (it != gl_ctx->buffers.end()) {
        glDeleteBuffers(1, &it->second);
        gl_ctx->buffers.erase(it);
    }
}

// -------------------------
// Shader Functions
// -------------------------
static ShaderHandle GL_CreateShader(void *ctx, const ShaderDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);

    GLenum shaderType = GL_VERTEX_SHADER;
    if (desc.stage == ShaderStage::Fragment)
        shaderType = GL_FRAGMENT_SHADER;
    // Extend this switch for Compute, Geometry, etc., as needed.

    GLuint shader = glCreateShader(shaderType);
    const GLchar *source = static_cast<const GLchar *>(desc.code);
    // Note: using code_size as a pointer to length is not typical.
    // Here we assume the code is null-terminated. In a real implementation,
    // you might pass an array of lengths.
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compilation error: " << log << "\n";
        glDeleteShader(shader);
        return ShaderHandle{};
    }

    ShaderHandle handle;
    handle.id = gl_ctx->next_shader_id++;
    gl_ctx->shaders[handle.id] = shader;
    return handle;
}

static void GL_DestroyShader(void *ctx, ShaderHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    auto it = gl_ctx->shaders.find(handle.id);
    if (it != gl_ctx->shaders.end()) {
        glDeleteShader(it->second);
        gl_ctx->shaders.erase(it);
    }
}

// -------------------------
// Pipeline Functions
// -------------------------
static PipelineHandle GL_CreatePipeline(void *ctx, const PipelineDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    GLuint program = glCreateProgram();

    if (desc.vertex_shader.id != 0)
        glAttachShader(program, gl_ctx->shaders[desc.vertex_shader.id]);
    if (desc.fragment_shader.id != 0)
        glAttachShader(program, gl_ctx->shaders[desc.fragment_shader.id]);

    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program link error: " << log << "\n";
        glDeleteProgram(program);
        return PipelineHandle{};
    }

    // Detach shaders (optional)
    if (desc.vertex_shader.id != 0)
        glDetachShader(program, gl_ctx->shaders[desc.vertex_shader.id]);
    if (desc.fragment_shader.id != 0)
        glDetachShader(program, gl_ctx->shaders[desc.fragment_shader.id]);

    PipelineHandle handle;
    handle.id = gl_ctx->next_pipeline_id++;
    gl_ctx->pipelines[handle.id] = program;
    return handle;
}

static void GL_DestroyPipeline(void *ctx, PipelineHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    auto it = gl_ctx->pipelines.find(handle.id);
    if (it != gl_ctx->pipelines.end()) {
        glDeleteProgram(it->second);
        gl_ctx->pipelines.erase(it);
    }
}

// -------------------------
// Texture Functions
// -------------------------
static TextureHandle GL_CreateTexture(void *ctx, const TextureDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Map abstract Format to GL internal format, format, and type.
    GLenum internalFormat = GL_RGBA8;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
    switch (desc.format) {
        case Format::R8:     internalFormat = GL_R8;  format = GL_RED; break;
        case Format::RG8:    internalFormat = GL_RG8; format = GL_RG; break;
        case Format::RGB8:   internalFormat = GL_RGB8; format = GL_RGB; break;
        case Format::RGBA8:  internalFormat = GL_RGBA8; format = GL_RGBA; break;
        case Format::RGBA16F:internalFormat = GL_RGBA16F; format = GL_RGBA; type = GL_HALF_FLOAT; break;
        case Format::RGBA32F:internalFormat = GL_RGBA32F; format = GL_RGBA; type = GL_FLOAT; break;
        default: break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, desc.width, desc.height, 0, format, type, nullptr);
    // Set filtering parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (desc.min_filter == Filter::Nearest) ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (desc.mag_filter == Filter::Nearest) ? GL_NEAREST : GL_LINEAR);
    // Set wrapping modes.
    auto wrapMode = [](AddressMode mode) -> GLint {
        switch (mode) {
            case AddressMode::Repeat:         return GL_REPEAT;
            case AddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
            case AddressMode::ClampToEdge:      return GL_CLAMP_TO_EDGE;
            case AddressMode::ClampToBorder:    return GL_CLAMP_TO_BORDER;
            default:                          return GL_REPEAT;
        }
    };
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode(desc.address_mode_u));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode(desc.address_mode_v));
    glBindTexture(GL_TEXTURE_2D, 0);

    TextureHandle handle;
    handle.id = gl_ctx->next_texture_id++;
    gl_ctx->textures[handle.id] = texture;
    return handle;
}

static void GL_DestroyTexture(void *ctx, TextureHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    auto it = gl_ctx->textures.find(handle.id);
    if (it != gl_ctx->textures.end()) {
        glDeleteTextures(1, &it->second);
        gl_ctx->textures.erase(it);
    }
}

// -------------------------
// Sampler Functions
// -------------------------
static SamplerHandle GL_CreateSampler(void *ctx, const SamplerDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    GLuint sampler = 0;
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, (desc.min_filter == Filter::Nearest) ? GL_NEAREST : GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, (desc.mag_filter == Filter::Nearest) ? GL_NEAREST : GL_LINEAR);

    auto wrapMode = [](AddressMode mode) -> GLint {
        switch (mode) {
            case AddressMode::Repeat:         return GL_REPEAT;
            case AddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
            case AddressMode::ClampToEdge:      return GL_CLAMP_TO_EDGE;
            case AddressMode::ClampToBorder:    return GL_CLAMP_TO_BORDER;
            default:                          return GL_REPEAT;
        }
    };
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrapMode(desc.address_mode_u));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrapMode(desc.address_mode_v));
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, wrapMode(desc.address_mode_w));

    float borderColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    if (desc.border_color == BorderColor::OpaqueBlack)
        borderColor[3] = 1.0f;
    else if (desc.border_color == BorderColor::OpaqueWhite)
        borderColor[0] = borderColor[1] = borderColor[2] = borderColor[3] = 1.0f;
    glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, borderColor);

    SamplerHandle handle;
    handle.id = gl_ctx->next_sampler_id++;
    gl_ctx->samplers[handle.id] = sampler;
    return handle;
}

static void GL_DestroySampler(void *ctx, SamplerHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    auto it = gl_ctx->samplers.find(handle.id);
    if (it != gl_ctx->samplers.end()) {
        glDeleteSamplers(1, &it->second);
        gl_ctx->samplers.erase(it);
    }
}

// -------------------------
// Render Pass & Framebuffer Functions
// (Note: Render passes are a Vulkan concept; here we create dummies.)
// -------------------------
static RenderPassHandle GL_CreateRenderPass(void *ctx, const RenderPassDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    RenderPassHandle handle;
    handle.id = gl_ctx->next_renderpass_id++;
    gl_ctx->renderpasses[handle.id] = 1;  // Dummy value.
    return handle;
}

static void GL_DestroyRenderPass(void *ctx, RenderPassHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    gl_ctx->renderpasses.erase(handle.id);
}

static FramebufferHandle GL_CreateFramebuffer(void *ctx, const FramebufferDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // For each attachment, attach the texture (assuming 2D textures).
    for (size_t i = 0; i < desc.attachments.size(); ++i) {
        const auto &att = desc.attachments[i];
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i),
                               GL_TEXTURE_2D,
                               att.texture.id,
                               0);
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer incomplete!\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    FramebufferHandle handle;
    handle.id = gl_ctx->next_framebuffer_id++;
    gl_ctx->framebuffers[handle.id] = framebuffer;
    return handle;
}

static void GL_DestroyFramebuffer(void *ctx, FramebufferHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    auto it = gl_ctx->framebuffers.find(handle.id);
    if (it != gl_ctx->framebuffers.end()) {
        glDeleteFramebuffers(1, &it->second);
        gl_ctx->framebuffers.erase(it);
    }
}

// -------------------------
// Descriptor Set Layout & Descriptor Set Functions (Dummies)
// -------------------------
static DescriptorSetLayoutHandle GL_CreateDescriptorSetLayout(void *ctx, const DescriptorSetLayoutDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    DescriptorSetLayoutHandle handle;
    handle.id = gl_ctx->next_descriptor_set_layout_id++;
    gl_ctx->descriptor_set_layouts[handle.id] = 1;  // Dummy value.
    return handle;
}

static void GL_DestroyDescriptorSetLayout(void *ctx, DescriptorSetLayoutHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    gl_ctx->descriptor_set_layouts.erase(handle.id);
}

static DescriptorSetHandle GL_CreateDescriptorSet(void *ctx, const DescriptorSetDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    DescriptorSetHandle handle;
    handle.id = gl_ctx->next_descriptor_set_id++;
    gl_ctx->descriptor_sets[handle.id] = 1;  // Dummy value.
    return handle;
}

static void GL_DestroyDescriptorSet(void *ctx, DescriptorSetHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    gl_ctx->descriptor_sets.erase(handle.id);
}

// -------------------------
// Command Buffer Functions (Dummy, immediate mode)
// -------------------------
static CommandBufferHandle GL_CreateCommandBuffer(void *ctx, const CommandBufferDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    CommandBufferHandle handle;
    handle.id = gl_ctx->next_command_buffer_id++;
    gl_ctx->command_buffers[handle.id] = 1;  // Dummy
    return handle;
}

static void GL_DestroyCommandBuffer(void *ctx, CommandBufferHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    gl_ctx->command_buffers.erase(handle.id);
}

// -------------------------
// Fence Functions
// -------------------------
static FenceHandle GL_CreateFence(void *ctx) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    FenceHandle handle;
    GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    handle.id = gl_ctx->next_fence_id++;
    gl_ctx->fences[handle.id] = sync;
    return handle;
}

static void GL_DestroyFence(void *ctx, FenceHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    auto it = gl_ctx->fences.find(handle.id);
    if (it != gl_ctx->fences.end()) {
        glDeleteSync(it->second);
        gl_ctx->fences.erase(it);
    }
}

// -------------------------
// Semaphore Functions (Dummy)
// -------------------------
static SemaphoreHandle GL_CreateSemaphore(void *ctx) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    SemaphoreHandle handle;
    handle.id = gl_ctx->next_semaphore_id++;
    gl_ctx->semaphores[handle.id] = 1;  // Dummy
    return handle;
}

static void GL_DestroySemaphore(void *ctx, SemaphoreHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    gl_ctx->semaphores.erase(handle.id);
}

// -------------------------
// Swapchain Functions (Dummy)
// -------------------------
static SwapchainHandle GL_CreateSwapchain(void *ctx, const SwapchainDesc &desc) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    SwapchainHandle handle;
    handle.id = gl_ctx->next_swapchain_id++;
    gl_ctx->swapchains[handle.id] = 1;  // Dummy
    return handle;
}

static void GL_DestroySwapchain(void *ctx, SwapchainHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    gl_ctx->swapchains.erase(handle.id);
}

// -------------------------
// Other Device Functions
// -------------------------
static bool GL_IsFeatureSupported(void * /*ctx*/, DeviceFeature /*feature*/) {
    // For simplicity, assume all basic features are supported.
    return true;
}

static DeviceErrorType GL_GetLastErrorType(void * /*ctx*/) {
    return DeviceErrorType::None;
}

static void GL_ClearLastError(void * /*ctx*/) {
    // No-op.
}

static void GL_DeviceBarrier(void * /*ctx*/,
                               PipelineStage /*src_stage*/,
                               PipelineStage /*dst_stage*/,
                               AccessFlags /*src_access*/,
                               AccessFlags /*dst_access*/) {
    // For simplicity, no explicit barrier is implemented.
}

// -------------------------
// Buffer Mapping and Updating
// -------------------------
static void* GL_MapBuffer(void *ctx, BufferHandle handle, Uint32 offset, Uint32 size, AccessFlags flags) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    auto it = gl_ctx->buffers.find(handle.id);
    if (it == gl_ctx->buffers.end()) {
        throw std::runtime_error("Buffer not found!");
    }
    GLenum access = GL_READ_WRITE;
    if ((static_cast<Uint32>(flags) & static_cast<Uint32>(AccessFlags::Read)) &&
        !(static_cast<Uint32>(flags) & static_cast<Uint32>(AccessFlags::Write)))
        access = GL_READ_ONLY;
    else if (!(static_cast<Uint32>(flags) & static_cast<Uint32>(AccessFlags::Read)) &&
              (static_cast<Uint32>(flags) & static_cast<Uint32>(AccessFlags::Write)))
        access = GL_WRITE_ONLY;

    glBindBuffer(GL_ARRAY_BUFFER, it->second);
    void* ptr = glMapBufferRange(GL_ARRAY_BUFFER, offset, size, access);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return ptr;
}

static void GL_UnmapBuffer(void *ctx, BufferHandle handle) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    auto it = gl_ctx->buffers.find(handle.id);
    if (it != gl_ctx->buffers.end()) {
        glBindBuffer(GL_ARRAY_BUFFER, it->second);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

static void GL_UpdateBuffer(void *ctx, BufferHandle handle, const void* data, Uint32 offset, Uint32 size) {
    GLDeviceContext *gl_ctx = static_cast<GLDeviceContext *>(ctx);
    auto it = gl_ctx->buffers.find(handle.id);
    if (it != gl_ctx->buffers.end()) {
        glBindBuffer(GL_ARRAY_BUFFER, it->second);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// -------------------------
// Multi-draw Indirect Functions
// -------------------------
static void GL_MultiDrawIndirect(void *ctx, Uint32 mode, Uint32 type, const void* indirect, Uint32 draw_count, Uint32 stride) {
    glMultiDrawElementsIndirect(mode, type, indirect, draw_count, stride);
}

static void GL_MultiDrawArraysIndirect(void *ctx, Uint32 mode, const void* indirect, Uint32 draw_count, Uint32 stride) {
    glMultiDrawArraysIndirect(mode, indirect, draw_count, stride);
}

// -------------------------
// Create DeviceFunctions Table
// -------------------------
static DeviceFunctions CreateGLBackendFunctions() {
    DeviceFunctions funcs = {};
    funcs.create_buffer                = GL_CreateBuffer;
    funcs.destroy_buffer               = GL_DestroyBuffer;
    funcs.create_shader                = GL_CreateShader;
    funcs.destroy_shader               = GL_DestroyShader;
    funcs.create_pipeline              = GL_CreatePipeline;
    funcs.destroy_pipeline             = GL_DestroyPipeline;
    funcs.create_texture               = GL_CreateTexture;
    funcs.destroy_texture              = GL_DestroyTexture;
    funcs.create_sampler               = GL_CreateSampler;
    funcs.destroy_sampler              = GL_DestroySampler;
    funcs.create_render_pass           = GL_CreateRenderPass;
    funcs.destroy_render_pass          = GL_DestroyRenderPass;
    funcs.create_framebuffer           = GL_CreateFramebuffer;
    funcs.destroy_framebuffer          = GL_DestroyFramebuffer;
    funcs.create_descriptor_set_layout = GL_CreateDescriptorSetLayout;
    funcs.destroy_descriptor_set_layout= GL_DestroyDescriptorSetLayout;
    funcs.create_descriptor_set        = GL_CreateDescriptorSet;
    funcs.destroy_descriptor_set       = GL_DestroyDescriptorSet;
    funcs.create_command_buffer        = GL_CreateCommandBuffer;
    funcs.destroy_command_buffer       = GL_DestroyCommandBuffer;
    funcs.create_fence                 = GL_CreateFence;
    funcs.destroy_fence                = GL_DestroyFence;
    funcs.create_semaphore             = GL_CreateSemaphore;
    funcs.destroy_semaphore            = GL_DestroySemaphore;
    funcs.create_swapchain             = GL_CreateSwapchain;
    funcs.destroy_swapchain            = GL_DestroySwapchain;
    funcs.device_barrier               = GL_DeviceBarrier;
    funcs.is_feature_supported         = GL_IsFeatureSupported;
    funcs.get_last_error_type          = GL_GetLastErrorType;
    funcs.clear_last_error             = GL_ClearLastError;
    funcs.map_buffer                   = GL_MapBuffer;
    funcs.unmap_buffer                 = GL_UnmapBuffer;
    funcs.update_buffer                = GL_UpdateBuffer;
    funcs.multi_draw_indirect          = GL_MultiDrawIndirect;
    funcs.multi_draw_arrays_indirect   = GL_MultiDrawArraysIndirect;
    return funcs;
}

} // namespace Bcg::Graphics
