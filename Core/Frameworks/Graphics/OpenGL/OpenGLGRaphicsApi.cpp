#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include "GraphicsApi.h" // The abstract API header from previous steps
#include <glad/gl.h> // or your chosen GL loader

namespace Bcg::Graphics {

struct GLDeviceContext {
    Uint32 next_buffer_id = 1;
    std::unordered_map<Uint32, GLuint> buffers;

    Uint32 next_shader_id = 1;
    std::unordered_map<Uint32, GLuint> shaders;

    Uint32 next_pipeline_id = 1;
    std::unordered_map<Uint32, GLuint> pipelines;

    // Add more maps for textures, samplers, etc. as needed.
};

// Convert BufferUsage enum to GL usage hint (simplified)
static GLenum GLUsageFromBufferUsage(BufferUsage usage) {
    switch (usage) {
        case BufferUsage::VertexBuffer:  return GL_ARRAY_BUFFER;
        case BufferUsage::IndexBuffer:   return GL_ELEMENT_ARRAY_BUFFER;
        case BufferUsage::UniformBuffer: return GL_UNIFORM_BUFFER;
        case BufferUsage::StorageBuffer: return GL_SHADER_STORAGE_BUFFER;
        default: return GL_ARRAY_BUFFER;
    }
}

// Simple function to create a buffer
static BufferHandle GL_CreateBuffer(void* ctx, const BufferDesc& desc) {
    GLDeviceContext* gl_ctx = static_cast<GLDeviceContext*>(ctx);
    GLuint gl_buffer = 0;
    glCreateBuffers(1, &gl_buffer);
    // Just allocate the storage; dynamic usage for simplicity
    glNamedBufferData(gl_buffer, desc.size, nullptr, GL_DYNAMIC_DRAW);

    BufferHandle handle;
    handle.id = gl_ctx->next_buffer_id++;
    gl_ctx->buffers[handle.id] = gl_buffer;
    return handle;
}

static void GL_DestroyBuffer(void* ctx, BufferHandle handle) {
    GLDeviceContext* gl_ctx = static_cast<GLDeviceContext*>(ctx);
    auto it = gl_ctx->buffers.find(handle.id);
    if (it != gl_ctx->buffers.end()) {
        glDeleteBuffers(1, &it->second);
        gl_ctx->buffers.erase(it);
    }
}

// Minimal Shader Creation (for a vertex/fragment shader)
static ShaderHandle GL_CreateShader(void* ctx, const ShaderDesc& desc) {
    GLDeviceContext* gl_ctx = static_cast<GLDeviceContext*>(ctx);

    GLenum shaderType = GL_VERTEX_SHADER;
    if (desc.stage == ShaderStage::Fragment) {
        shaderType = GL_FRAGMENT_SHADER;
    }

    GLuint shader = glCreateShader(shaderType);
    const GLchar* source = static_cast<const GLchar*>(desc.code);
    glShaderSource(shader, 1, &source, (const GLint*)&desc.code_size);
    glCompileShader(shader);

    // Check for errors:
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compilation error: " << log << "\n";
        glDeleteShader(shader);
        ShaderHandle invalid{};
        return invalid;
    }

    ShaderHandle handle;
    handle.id = gl_ctx->next_shader_id++;
    gl_ctx->shaders[handle.id] = shader;
    return handle;
}

static void GL_DestroyShader(void* ctx, ShaderHandle handle) {
    GLDeviceContext* gl_ctx = static_cast<GLDeviceContext*>(ctx);
    auto it = gl_ctx->shaders.find(handle.id);
    if (it != gl_ctx->shaders.end()) {
        glDeleteShader(it->second);
        gl_ctx->shaders.erase(it);
    }
}

// Minimal Pipeline Creation: Just link a vertex and fragment shader
static PipelineHandle GL_CreatePipeline(void* ctx, const PipelineDesc& desc) {
    GLDeviceContext* gl_ctx = static_cast<GLDeviceContext*>(ctx);

    // A pipeline in OpenGL is essentially a linked program
    GLuint program = glCreateProgram();
    if (desc.vertex_shader.id != 0) {
        glAttachShader(program, gl_ctx->shaders[desc.vertex_shader.id]);
    }
    if (desc.fragment_shader.id != 0) {
        glAttachShader(program, gl_ctx->shaders[desc.fragment_shader.id]);
    }
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program link error: " << log << "\n";
        glDeleteProgram(program);
        PipelineHandle invalid{};
        return invalid;
    }

    PipelineHandle handle;
    handle.id = gl_ctx->next_pipeline_id++;
    gl_ctx->pipelines[handle.id] = program;
    return handle;
}

static void GL_DestroyPipeline(void* ctx, PipelineHandle handle) {
    GLDeviceContext* gl_ctx = static_cast<GLDeviceContext*>(ctx);
    auto it = gl_ctx->pipelines.find(handle.id);
    if (it != gl_ctx->pipelines.end()) {
        glDeleteProgram(it->second);
        gl_ctx->pipelines.erase(it);
    }
}

// For simplicity, we won't implement other functions like textures, render passes, etc.
// We'll just implement what's needed to render a simple triangle.

static bool GL_IsFeatureSupported(void* /*ctx*/, DeviceFeature /*feature*/) {
    // For this example, return true for basic features or false otherwise.
    return true;
}

static DeviceErrorType GL_GetLastErrorType(void* /*ctx*/) {
    // For simplicity, assume no errors.
    return DeviceErrorType::None;
}

static void GL_ClearLastError(void* /*ctx*/) {
    // No-op in this example.
}

static void GL_DeviceBarrier(void* /*ctx*/,
                             PipelineStage /*src_stage*/,
                             PipelineStage /*dst_stage*/,
                             AccessFlags /*src_access*/,
                             AccessFlags /*dst_access*/) {
    // OpenGL generally doesn't use explicit memory barriers like Vulkan for this scenario.
    // In a real implementation, you might use glMemoryBarrier() depending on usage.
}

static CommandBufferHandle GL_CreateCommandBuffer(void* /*ctx*/, const CommandBufferDesc& /*desc*/) {
    // For simplicity, we won't implement command buffers. Assume immediate mode.
    CommandBufferHandle handle{};
    handle.id = 1; // Dummy
    return handle;
}
static void GL_DestroyCommandBuffer(void* /*ctx*/, CommandBufferHandle /*handle*/) {}

// Fences, semaphores, swapchains, etc., are not implemented here for brevity.
// We'll assume a simple rendering scenario directly to the default framebuffer.


static DeviceFunctions CreateGLBackendFunctions() {
    DeviceFunctions funcs = {};
    funcs.CreateBuffer = GL_CreateBuffer;
    funcs.DestroyBuffer = GL_DestroyBuffer;
    funcs.CreateShader = GL_CreateShader;
    funcs.DestroyShader = GL_DestroyShader;
    funcs.CreatePipeline = GL_CreatePipeline;
    funcs.DestroyPipeline = GL_DestroyPipeline;
    funcs.IsFeatureSupported = GL_IsFeatureSupported;
    funcs.GetLastErrorType = GL_GetLastErrorType;
    funcs.ClearLastError = GL_ClearLastError;
    funcs.DeviceBarrier = GL_DeviceBarrier;
    funcs.CreateCommandBuffer = GL_CreateCommandBuffer;
    funcs.DestroyCommandBuffer = GL_DestroyCommandBuffer;
    // Other functions left as nullptr for now.
    return funcs;
}
}