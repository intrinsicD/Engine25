// OpenGLBackend.cpp
// An example implementation of the abstract graphics API using OpenGL

#include <cstdio>
#include <vector>
#include <glad/gl.h>
#include "GraphicsTypes.h"  // Assumed to contain types such as Uint32, SizeType, etc.
#include "GraphicsApi.h"  // The header from your provided API
#include "OpenGLBackend.h"  // The header from your provided API
#include <iostream>

namespace Bcg {
    namespace Graphics {
        void MyOpenGLDebugCallback(GLenum source,
                                   GLenum type,
                                   GLuint id,
                                   GLenum severity,
                                   GLsizei length,
                                   const GLchar *message,
                                   const void *userParam) {
            // You can add more sophisticated filtering based on source, type, or severity.
            std::cerr << "OpenGL Debug Message: " << message << std::endl;
        }

        void EnableDebugOutput(OpenGLContext *context) {
            // Check that the function pointer is valid (GLAD should have loaded it).
            if (glDebugMessageCallback) {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                glDebugMessageCallback(MyOpenGLDebugCallback, nullptr);
                // Optionally, control which messages you want to receive.
                glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

                context->debugOutputEnabled = true;
            } else {
                context->debugOutputEnabled = false;
            }
        }

        namespace OpenGL {
            // Helper functions to convert API enums to OpenGL enums.
            static GLenum to_gl_buffer_target(BufferUsage usage) {
                switch (usage) {
                    case BufferUsage::VertexBuffer: return GL_ARRAY_BUFFER;
                    case BufferUsage::IndexBuffer: return GL_ELEMENT_ARRAY_BUFFER;
                    case BufferUsage::UniformBuffer: return GL_UNIFORM_BUFFER;
                    case BufferUsage::StorageBuffer: return GL_SHADER_STORAGE_BUFFER;
                    default: return GL_ARRAY_BUFFER;
                }
            }

            static GLenum to_gl_texture_target(TextureType type) {
                switch (type) {
                    case TextureType::Texture1D: return GL_TEXTURE_1D;
                    case TextureType::Texture2D: return GL_TEXTURE_2D;
                    case TextureType::Texture3D: return GL_TEXTURE_3D;
                    case TextureType::CubeMap: return GL_TEXTURE_CUBE_MAP;
                    case TextureType::TextureArray: return GL_TEXTURE_2D_ARRAY;
                    default: return GL_TEXTURE_2D;
                }
            }

            static GLenum to_gl_internal_format(Format format) {
                switch (format) {
                    case Format::R8: return GL_R8;
                    case Format::RG8: return GL_RG8;
                    case Format::RGB8: return GL_RGB8;
                    case Format::RGBA8: return GL_RGBA8;
                    case Format::RGBA16F: return GL_RGBA16F;
                    case Format::RGBA32F: return GL_RGBA32F;
                    case Format::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
                    case Format::Depth32F: return GL_DEPTH_COMPONENT32F;
                    default: return GL_RGBA8;
                }
            }

            static GLenum to_gl_filter(Filter filter) {
                return filter == Filter::Nearest ? GL_NEAREST : GL_LINEAR;
            }

            static GLenum to_gl_address_mode(AddressMode mode) {
                switch (mode) {
                    case AddressMode::Repeat: return GL_REPEAT;
                    case AddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
                    case AddressMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
                    case AddressMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
                    default: return GL_REPEAT;
                }
            }

            //
            // Buffer creation/destruction
            //
            BufferHandle create_buffer(void * /*ctx*/, const BufferDesc &desc) {
                BufferHandle handle;
                GLuint buffer;
                glGenBuffers(1, &buffer);
                handle.id = buffer;
                GLenum target = to_gl_buffer_target(desc.usage);
                glBindBuffer(target, buffer);
                // For simplicity, always using GL_STATIC_DRAW here.
                glBufferData(target, desc.size, nullptr, GL_STATIC_DRAW);
                glBindBuffer(target, 0);
                return handle;
            }

            void destroy_buffer(void * /*ctx*/, BufferHandle handle) {
                GLuint buffer = handle.id;
                glDeleteBuffers(1, &buffer);
            }

            //
            // Texture creation/destruction
            //
            TextureHandle create_texture(void * /*ctx*/, const TextureDesc &desc) {
                TextureHandle handle;
                GLuint texture;
                glGenTextures(1, &texture);
                handle.id = texture;
                GLenum target = to_gl_texture_target(desc.type);
                glBindTexture(target, texture);

                GLenum internal_format = to_gl_internal_format(desc.format);
                // For demonstration, assuming a 2D texture.
                if (target == GL_TEXTURE_2D) {
                    // Using GL_RGBA and GL_UNSIGNED_BYTE as example external format/type.
                    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, desc.width, desc.height, 0, GL_RGBA,
                                 GL_UNSIGNED_BYTE, nullptr);
                }
                // Set texture parameters.
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, to_gl_filter(desc.min_filter));
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, to_gl_filter(desc.mag_filter));
                glTexParameteri(target, GL_TEXTURE_WRAP_S, to_gl_address_mode(desc.address_mode_u));
                glTexParameteri(target, GL_TEXTURE_WRAP_T, to_gl_address_mode(desc.address_mode_v));
                glBindTexture(target, 0);
                return handle;
            }

            void destroy_texture(void * /*ctx*/, TextureHandle handle) {
                GLuint texture = handle.id;
                glDeleteTextures(1, &texture);
            }

            //
            // Sampler creation/destruction
            //
            SamplerHandle create_sampler(void * /*ctx*/, const SamplerDesc &desc) {
                SamplerHandle handle;
                GLuint sampler;
                glGenSamplers(1, &sampler);
                handle.id = sampler;
                glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, to_gl_filter(desc.min_filter));
                glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, to_gl_filter(desc.mag_filter));
                glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, to_gl_address_mode(desc.address_mode_u));
                glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, to_gl_address_mode(desc.address_mode_v));
                glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, to_gl_address_mode(desc.address_mode_w));
                return handle;
            }

            void destroy_sampler(void * /*ctx*/, SamplerHandle handle) {
                GLuint sampler = handle.id;
                glDeleteSamplers(1, &sampler);
            }

            //
            // Shader creation/destruction
            //
            static GLenum to_gl_shader_type(ShaderStage stage) {
                switch (stage) {
                    case ShaderStage::Vertex: return GL_VERTEX_SHADER;
                    case ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
                    case ShaderStage::Compute: return GL_COMPUTE_SHADER;
                    case ShaderStage::Geometry: return GL_GEOMETRY_SHADER;
                    case ShaderStage::TessellationControl: return GL_TESS_CONTROL_SHADER;
                    case ShaderStage::TessellationEvaluation: return GL_TESS_EVALUATION_SHADER;
                    default: return GL_VERTEX_SHADER;
                }
            }

            ShaderHandle create_shader(void * /*ctx*/, const ShaderDesc &desc) {
                ShaderHandle handle;
                GLuint shader = glCreateShader(to_gl_shader_type(desc.stage));
                handle.id = shader;
                // Assume 'code' is a null-terminated GLSL source string.
                const char *source = reinterpret_cast<const char *>(desc.code);
                glShaderSource(shader, 1, &source, nullptr);
                glCompileShader(shader);
                // Check for compilation errors.
                GLint compiled;
                glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
                if (!compiled) {
                    char infoLog[512];
                    glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
                    fprintf(stderr, "Shader compilation error: %s\n", infoLog);
                }
                return handle;
            }

            void destroy_shader(void * /*ctx*/, ShaderHandle handle) {
                glDeleteShader(handle.id);
            }

            //
            // Pipeline creation/destruction (link shaders into a program)
            //
            PipelineHandle create_pipeline(void * /*ctx*/, const PipelineDesc &desc) {
                PipelineHandle handle;
                GLuint program = glCreateProgram();
                if (desc.vertex_shader.id != InvalidId) {
                    glAttachShader(program, desc.vertex_shader.id);
                }
                if (desc.fragment_shader.id != InvalidId) {
                    glAttachShader(program, desc.fragment_shader.id);
                }
                if (desc.geometry_shader.id != InvalidId) {
                    glAttachShader(program, desc.geometry_shader.id);
                }
                if (desc.tessellation_control_shader.id != InvalidId) {
                    glAttachShader(program, desc.tessellation_control_shader.id);
                }
                if (desc.tessellation_evaluation_shader.id != InvalidId) {
                    glAttachShader(program, desc.tessellation_evaluation_shader.id);
                }
                glLinkProgram(program);
                GLint linked;
                glGetProgramiv(program, GL_LINK_STATUS, &linked);
                if (!linked) {
                    char infoLog[512];
                    glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
                    fprintf(stderr, "Program link error: %s\n", infoLog);
                }
                handle.id = program;
                return handle;
            }

            void destroy_pipeline(void * /*ctx*/, PipelineHandle handle) {
                glDeleteProgram(handle.id);
            }

            //
            // Render pass creation/destruction (dummy implementation)
            //
            RenderPassHandle create_render_pass(void * /*ctx*/, const RenderPassDesc &/*desc*/) {
                RenderPassHandle handle;
                static Uint32 nextId = 1;
                handle.id = nextId++;
                return handle;
            }

            void destroy_render_pass(void * /*ctx*/, RenderPassHandle /*handle*/) {
                // No operation (render passes are not explicit in OpenGL)
            }

            //
            // Framebuffer creation/destruction
            //
            FramebufferHandle create_framebuffer(void * /*ctx*/, const FramebufferDesc &desc) {
                FramebufferHandle handle;
                GLuint fbo;
                glGenFramebuffers(1, &fbo);
                handle.id = fbo;
                glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                // For simplicity, attach each provided texture as a color attachment.
                for (size_t i = 0; i < desc.attachments.size(); ++i) {
                    const FramebufferAttachmentDesc &att = desc.attachments[i];
                    glFramebufferTexture2D(GL_FRAMEBUFFER,
                                           GL_COLOR_ATTACHMENT0 + static_cast<GLuint>(i),
                                           GL_TEXTURE_2D, att.texture.id, att.mip_level);
                }
                GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (status != GL_FRAMEBUFFER_COMPLETE) {
                    fprintf(stderr, "Framebuffer incomplete: 0x%x\n", status);
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                return handle;
            }

            void destroy_framebuffer(void * /*ctx*/, FramebufferHandle handle) {
                GLuint fbo = handle.id;
                glDeleteFramebuffers(1, &fbo);
            }

            //
            // Descriptor set layout and descriptor set (dummy implementations)
            //
            DescriptorSetLayoutHandle create_descriptor_set_layout(void * /*ctx*/,
                                                                   const DescriptorSetLayoutDesc &/*desc*/) {
                DescriptorSetLayoutHandle handle;
                static Uint32 nextId = 1;
                handle.id = nextId++;
                return handle;
            }

            void destroy_descriptor_set_layout(void * /*ctx*/, DescriptorSetLayoutHandle /*handle*/) {
                // No operation.
            }

            DescriptorSetHandle create_descriptor_set(void * /*ctx*/, const DescriptorSetDesc &/*desc*/) {
                DescriptorSetHandle handle;
                static Uint32 nextId = 1;
                handle.id = nextId++;
                return handle;
            }

            void destroy_descriptor_set(void * /*ctx*/, DescriptorSetHandle /*handle*/) {
                // No operation.
            }

            //
            // Command buffer creation/destruction (dummy as OpenGL is immediate)
            //
            CommandBufferHandle create_command_buffer(void * /*ctx*/, const CommandBufferDesc &/*desc*/) {
                CommandBufferHandle handle;
                static Uint32 nextId = 1;
                handle.id = nextId++;
                return handle;
            }

            void destroy_command_buffer(void * /*ctx*/, CommandBufferHandle /*handle*/) {
                // No operation.
            }

            //
            // Fence creation/destruction using OpenGL sync objects
            //
            FenceHandle create_fence(void * /*ctx*/) {
                FenceHandle handle;
                GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
                handle.id = reinterpret_cast<Uint64>(sync);
                return handle;
            }

            void destroy_fence(void * /*ctx*/, FenceHandle handle) {
                GLsync sync = reinterpret_cast<GLsync>(handle.id);
                glDeleteSync(sync);
            }

            //
            // Semaphore creation/destruction (dummy, as OpenGL lacks explicit semaphores)
            //
            SemaphoreHandle create_semaphore(void * /*ctx*/) {
                SemaphoreHandle handle;
                static Uint32 nextId = 1;
                handle.id = nextId++;
                return handle;
            }

            void destroy_semaphore(void * /*ctx*/, SemaphoreHandle /*handle*/) {
                // No operation.
            }

            //
            // Swapchain creation/destruction (dummy, managed by windowing system)
            //
            SwapchainHandle create_swapchain(void * /*ctx*/, const SwapchainDesc &/*desc*/) {
                SwapchainHandle handle;
                static Uint32 nextId = 1;
                handle.id = nextId++;
                return handle;
            }

            void destroy_swapchain(void * /*ctx*/, SwapchainHandle /*handle*/) {
                // No operation.
            }

            //
            // Memory barrier using glMemoryBarrier
            //
            void device_barrier(void * /*ctx*/, PipelineStage /*src_stage*/, PipelineStage /*dst_stage*/,
                                AccessFlags /*src_access*/, AccessFlags /*dst_access*/) {
                glMemoryBarrier(GL_ALL_BARRIER_BITS);
            }

            //
            // Feature support query (simplified)
            //
            bool is_feature_supported(void * /*ctx*/, DeviceFeature feature) {
                switch (feature) {
                    case DeviceFeature::MultiDrawIndirect:
                    case DeviceFeature::ComputeShaders:
                    case DeviceFeature::GeometryShaders:
                        return true;
                    case DeviceFeature::RayTracing:
                        return false;
                    default:
                        return false;
                }
            }

            //
            // Error handling
            //
            DeviceErrorType get_last_error_type(void * /*ctx*/) {
                GLenum err = glGetError();
                if (err == GL_OUT_OF_MEMORY)
                    return DeviceErrorType::OutOfMemory;
                else if (err != GL_NO_ERROR)
                    return DeviceErrorType::InvalidOperation;
                return DeviceErrorType::None;
            }

            void clear_last_error(void * /*ctx*/) {
                while (glGetError() != GL_NO_ERROR) {
                }
            }

            //
            // Command buffer recording (no-ops for immediate mode OpenGL)
            //
            void begin_command_buffer(void * /*ctx*/, CommandBufferHandle /*cmd*/) {
            }

            void end_command_buffer(void * /*ctx*/, CommandBufferHandle /*cmd*/) {
            }

            //
            // Submit (immediate execution)
            //
            void submit(void * /*ctx*/, const SubmitInfo &/*info*/) {
                // In immediate mode OpenGL, commands are executed as called.
            }

            void wait_for_fences(void * /*ctx*/, const std::vector<FenceHandle> &fences, bool /*wait_all*/,
                                 Uint64 timeout) {
                for (const auto &fence: fences) {
                    GLsync sync = reinterpret_cast<GLsync>(fence.id);
                    GLenum result = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
                    if (result == GL_TIMEOUT_EXPIRED)
                        fprintf(stderr, "Fence wait timeout.\n");
                }
            }

            void reset_fences(void * /*ctx*/, const std::vector<FenceHandle> &/*fences*/) {
                // Fences in OpenGL are one-time objects.
            }

            //
            // Swapchain operations (dummy)
            //
            bool acquire_next_image(void * /*ctx*/, SwapchainHandle /*swapchain*/, Uint64 /*timeout*/,
                                    SemaphoreHandle /*semaphore*/, FenceHandle /*fence*/, Uint32 *image_index) {
                if (image_index) *image_index = 0;
                return true;
            }

            void present(void * /*ctx*/, const PresentInfo &/*info*/) {
                // Presentation is managed by the windowing system (e.g. GLFW or SDL).
            }

            //
            // Descriptor set updates (dummy)
            //
            void update_descriptor_sets(void * /*ctx*/, const std::vector<DescriptorUpdate> &/*updates*/) {
                // OpenGL binds resources directly.
            }

            //
            // Draw commands
            //
            void draw(void * /*ctx*/, Uint32 vertex_count, Uint32 /*instance_count*/, Uint32 first_vertex,
                      Uint32 /*first_instance*/) {
                glDrawArrays(GL_TRIANGLES, first_vertex, vertex_count);
            }

            void draw_indexed(void * /*ctx*/, Uint32 index_count, Uint32 /*instance_count*/, Uint32 first_index,
                              Int32 /*vertex_offset*/, Uint32 /*first_instance*/) {
                glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT,
                               reinterpret_cast<void *>(first_index * sizeof(GLuint)));
            }

            //
            // Multi-draw indirect (simulate with loop)
            //
            void multi_draw_indirect(void * /*ctx*/, Uint32 mode, Uint32 /*type*/, const void *indirect,
                                     Uint32 draw_count, Uint32 stride) {
                // Check if the modern function is available.
                if (glMultiDrawElementsIndirect) {
                    // Use the modern OpenGL 4.5 function.
                    glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, indirect, draw_count, stride);
                } else {
                    // Fallback: loop over each draw command.
                    const char *ptr = reinterpret_cast<const char *>(indirect);
                    for (Uint32 i = 0; i < draw_count; ++i) {
                        const DrawElementsIndirectCommand *cmd =
                                reinterpret_cast<const DrawElementsIndirectCommand *>(ptr + i * stride);
                        glDrawElementsInstanced(mode, cmd->count, GL_UNSIGNED_INT,
                                                reinterpret_cast<void *>(cmd->first_index * sizeof(GLuint)),
                                                cmd->instance_count);
                    }
                }
            }

            void multi_draw_arrays_indirect(void * /*ctx*/, Uint32 mode, const void *indirect,
                                            Uint32 draw_count, Uint32 stride) {
                if (glMultiDrawArraysIndirect) {
                    glMultiDrawArraysIndirect(mode, indirect, draw_count, stride);
                } else {
                    const char *ptr = reinterpret_cast<const char *>(indirect);
                    for (Uint32 i = 0; i < draw_count; ++i) {
                        const DrawArraysIndirectCommand *cmd =
                                reinterpret_cast<const DrawArraysIndirectCommand *>(ptr + i * stride);
                        glDrawArraysInstanced(mode, cmd->first, cmd->count, cmd->instance_count);
                    }
                }
            }

            //
            // Buffer mapping operations
            //
            void *map_buffer(void * /*ctx*/, BufferHandle handle, Uint32 offset, Uint32 size, AccessFlags /*flags*/) {
                GLenum target = GL_ARRAY_BUFFER; // Assume vertex buffer for simplicity.
                glBindBuffer(target, handle.id);
                void *ptr = glMapBufferRange(target, offset, size, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
                glBindBuffer(target, 0);
                return ptr;
            }

            void unmap_buffer(void * /*ctx*/, BufferHandle handle) {
                GLenum target = GL_ARRAY_BUFFER;
                glBindBuffer(target, handle.id);
                glUnmapBuffer(target);
                glBindBuffer(target, 0);
            }

            void update_buffer(void * /*ctx*/, BufferHandle handle, const void *data, Uint32 offset, Uint32 size) {
                GLenum target = GL_ARRAY_BUFFER;
                glBindBuffer(target, handle.id);
                glBufferSubData(target, offset, size, data);
                glBindBuffer(target, 0);
            }

            //
            // Compute dispatch commands
            //
            void dispatch(void * /*ctx*/, Uint32 group_count_x, Uint32 group_count_y, Uint32 group_count_z) {
                glDispatchCompute(group_count_x, group_count_y, group_count_z);
            }

            void dispatch_indirect(void * /*ctx*/, CommandBufferHandle /*cmd*/, BufferHandle indirect_buffer,
                                   Uint32 offset) {
                glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, indirect_buffer.id);
                glDispatchComputeIndirect(offset);
                glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
            }

            //
            // Pipeline binding and dynamic state commands
            //
            void bind_pipeline(void * /*ctx*/, CommandBufferHandle /*cmd*/, PipelineHandle pipeline) {
                glUseProgram(pipeline.id);
            }

            void bind_vertex_buffers(void * /*ctx*/, CommandBufferHandle /*cmd*/, Uint32 /*first_binding*/,
                                     const BufferHandle *buffers, const Uint32 * /*offsets*/, Uint32 count) {
                if (count > 0)
                    glBindBuffer(GL_ARRAY_BUFFER, buffers[0].id);
            }

            void bind_index_buffer(void * /*ctx*/, CommandBufferHandle /*cmd*/, BufferHandle buffer, Uint32 /*offset*/,
                                   IndexType /*index_type*/) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.id);
            }

            void set_viewport(void * /*ctx*/, CommandBufferHandle /*cmd*/, const Viewport &viewport) {
                glViewport(static_cast<GLint>(viewport.x), static_cast<GLint>(viewport.y),
                           static_cast<GLsizei>(viewport.width), static_cast<GLsizei>(viewport.height));
            }

            void set_scissor(void * /*ctx*/, CommandBufferHandle /*cmd*/, const Rect2D &rect) {
                glScissor(rect.x, rect.y, rect.width, rect.height);
            }

            void update_push_constants(void * /*ctx*/, CommandBufferHandle /*cmd*/, PipelineLayoutHandle /*layout*/,
                                       ShaderStage /*stage*/, Uint32 /*offset*/, Uint32 /*size*/,
                                       const void * /*data*/) {
                // OpenGL does not support push constants; this is a stub.
            }

            //
            // Resource copy operations
            //
            void copy_buffer(void * /*ctx*/, BufferHandle src, BufferHandle dst, Uint32 src_offset, Uint32 dst_offset,
                             Uint32 size) {
                glBindBuffer(GL_COPY_READ_BUFFER, src.id);
                glBindBuffer(GL_COPY_WRITE_BUFFER, dst.id);
                glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, src_offset, dst_offset, size);
                glBindBuffer(GL_COPY_READ_BUFFER, 0);
                glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            }

            void copy_texture(void * /*ctx*/, TextureHandle src, TextureHandle dst) {
                // If the extension glCopyImageSubData is available, use it.
                glCopyImageSubData(src.id, GL_TEXTURE_2D, 0, 0, 0, 0,
                                   dst.id, GL_TEXTURE_2D, 0, 0, 0, 0,
                                   0, 0, 0);
            }

            //
            // Query and debug utilities (queries are not fully implemented)
            //
            void reset_query_pool(void * /*ctx*/, QueryPoolHandle /*pool*/, Uint32 /*first_query*/,
                                  Uint32 /*query_count*/) {
            }

            void begin_query(void * /*ctx*/, CommandBufferHandle /*cmd*/, QueryPoolHandle /*pool*/, Uint32 /*query*/,
                             const char * /*name*/) {
            }

            void end_query(void * /*ctx*/, CommandBufferHandle /*cmd*/, QueryPoolHandle /*pool*/, Uint32 /*query*/) {
            }

            void get_query_results(void * /*ctx*/, QueryPoolHandle /*pool*/, Uint32 /*first_query*/,
                                   Uint32 /*query_count*/,
                                   void * /*results*/, Uint32 /*stride*/, bool /*wait*/) {
            }

            //
            // Debug marker commands using GL_KHR_debug
            //
            void begin_debug_marker(void * /*ctx*/, CommandBufferHandle /*cmd*/, const char *marker_name,
                                    const float /*color*/[4]) {
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, marker_name);
            }

            void end_debug_marker(void * /*ctx*/, CommandBufferHandle /*cmd*/) {
                glPopDebugGroup();
            }

            void insert_debug_marker(void * /*ctx*/, CommandBufferHandle /*cmd*/, const char *marker_name,
                                     const float /*color*/[4]) {
                glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
                                     GL_DEBUG_SEVERITY_NOTIFICATION, -1, marker_name);
            }

            //
            // Pipeline layout creation/destruction (dummy)
            //
            PipelineLayoutHandle create_pipeline_layout(void * /*ctx*/, const PipelineLayoutDesc &/*desc*/) {
                PipelineLayoutHandle handle;
                static Uint32 nextId = 1;
                handle.id = nextId++;
                return handle;
            }

            void destroy_pipeline_layout(void * /*ctx*/, PipelineLayoutHandle /*layout*/) {
                // No operation.
            }

            //
            // Compute pipeline creation/destruction (treat as a regular program)
            //
            PipelineHandle create_compute_pipeline(void * /*ctx*/, const ComputePipelineDesc &desc) {
                PipelineHandle handle;
                GLuint program = glCreateProgram();
                glAttachShader(program, desc.compute_shader.id);
                glLinkProgram(program);
                handle.id = program;
                return handle;
            }

            void destroy_compute_pipeline(void * /*ctx*/, PipelineHandle handle) {
                glDeleteProgram(handle.id);
            }

            //
            // Mesh pipeline creation/destruction (dummy: similar to graphics pipeline)
            //
            PipelineHandle create_mesh_pipeline(void * /*ctx*/, const MeshPipelineDesc &desc) {
                PipelineHandle handle;
                GLuint program = glCreateProgram();
                glAttachShader(program, desc.mesh_shader.id);
                glAttachShader(program, desc.fragment_shader.id);
                glLinkProgram(program);
                handle.id = program;
                return handle;
            }

            void destroy_mesh_pipeline(void * /*ctx*/, PipelineHandle handle) {
                glDeleteProgram(handle.id);
            }

            //
            // Texture view creation/destruction (if supported by GL_ARB_texture_view)
            //
            TextureViewHandle create_texture_view(void * /*ctx*/, const TextureViewDesc &desc) {
                TextureViewHandle handle;
#ifdef GL_ARB_texture_view
    glGenTextures(1, &handle.id);
    glTextureView(handle.id, GL_TEXTURE_2D, desc.texture.id,
                  to_gl_internal_format(desc.format),
                  desc.base_mip_level, desc.level_count,
                  desc.base_array_layer, desc.layer_count);
#else
                // Fallback: simply use the original texture.
                handle.id = desc.texture.id;
#endif
                return handle;
            }

            void destroy_texture_view(void * /*ctx*/, TextureViewHandle handle) {
#ifdef GL_ARB_texture_view
    glDeleteTextures(1, &handle.id);
#else
                // No operation.
#endif
            }

            //
            // Render pass commands
            //
            void begin_render_pass(void * /*ctx*/, CommandBufferHandle /*cmd*/, const RenderPassBeginInfo &begin_info) {
                // Bind the framebuffer.
                glBindFramebuffer(GL_FRAMEBUFFER, begin_info.framebuffer.id);
                // Set viewport and scissor according to the render area.
                glViewport(begin_info.render_area.x, begin_info.render_area.y,
                           begin_info.render_area.width, begin_info.render_area.height);
                glScissor(begin_info.render_area.x, begin_info.render_area.y,
                          begin_info.render_area.width, begin_info.render_area.height);
                // Clear attachments (for simplicity, assume color and depth).
                for (const auto &clear_val: begin_info.clear_values) {
                    glClearColor(clear_val.color.r, clear_val.color.g, clear_val.color.b, clear_val.color.a);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                }
            }

            void end_render_pass(void * /*ctx*/, CommandBufferHandle /*cmd*/) {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        } // namespace OpenGL

        //
        // Finally, instantiate a DeviceFunctions structure with pointers to our OpenGL implementations.
        //
        DeviceFunctions opengl_device_functions = {
            .create_buffer = OpenGL::create_buffer,
            .destroy_buffer = OpenGL::destroy_buffer,
            .create_texture = OpenGL::create_texture,
            .destroy_texture = OpenGL::destroy_texture,
            .create_sampler = OpenGL::create_sampler,
            .destroy_sampler = OpenGL::destroy_sampler,
            .create_shader = OpenGL::create_shader,
            .destroy_shader = OpenGL::destroy_shader,
            .create_pipeline = OpenGL::create_pipeline,
            .destroy_pipeline = OpenGL::destroy_pipeline,
            .create_render_pass = OpenGL::create_render_pass,
            .destroy_render_pass = OpenGL::destroy_render_pass,
            .create_framebuffer = OpenGL::create_framebuffer,
            .destroy_framebuffer = OpenGL::destroy_framebuffer,
            .create_descriptor_set_layout = OpenGL::create_descriptor_set_layout,
            .destroy_descriptor_set_layout = OpenGL::destroy_descriptor_set_layout,
            .create_descriptor_set = OpenGL::create_descriptor_set,
            .destroy_descriptor_set = OpenGL::destroy_descriptor_set,
            .create_command_buffer = OpenGL::create_command_buffer,
            .destroy_command_buffer = OpenGL::destroy_command_buffer,
            .create_fence = OpenGL::create_fence,
            .destroy_fence = OpenGL::destroy_fence,
            .create_semaphore = OpenGL::create_semaphore,
            .destroy_semaphore = OpenGL::destroy_semaphore,
            .create_swapchain = OpenGL::create_swapchain,
            .destroy_swapchain = OpenGL::destroy_swapchain,
            .device_barrier = OpenGL::device_barrier,
            .is_feature_supported = OpenGL::is_feature_supported,
            .get_last_error_type = OpenGL::get_last_error_type,
            .clear_last_error = OpenGL::clear_last_error,
            .begin_command_buffer = OpenGL::begin_command_buffer,
            .end_command_buffer = OpenGL::end_command_buffer,
            .submit = OpenGL::submit,
            .wait_for_fences = OpenGL::wait_for_fences,
            .reset_fences = OpenGL::reset_fences,
            .acquire_next_image = OpenGL::acquire_next_image,
            .present = OpenGL::present,
            .update_descriptor_sets = OpenGL::update_descriptor_sets,
            .draw = OpenGL::draw,
            .draw_indexed = OpenGL::draw_indexed,
            .multi_draw_indirect = OpenGL::multi_draw_indirect,
            .multi_draw_arrays_indirect = OpenGL::multi_draw_arrays_indirect,
            .map_buffer = OpenGL::map_buffer,
            .unmap_buffer = OpenGL::unmap_buffer,
            .update_buffer = OpenGL::update_buffer,
            .dispatch = OpenGL::dispatch,
            .dispatch_indirect = OpenGL::dispatch_indirect,
            .bind_pipeline = OpenGL::bind_pipeline,
            .bind_vertex_buffers = OpenGL::bind_vertex_buffers,
            .bind_index_buffer = OpenGL::bind_index_buffer,
            .set_viewport = OpenGL::set_viewport,
            .set_scissor = OpenGL::set_scissor,
            .update_push_constants = OpenGL::update_push_constants,
            .copy_buffer = OpenGL::copy_buffer,
            .copy_texture = OpenGL::copy_texture,
            .reset_query_pool = OpenGL::reset_query_pool,
            .begin_query = OpenGL::begin_query,
            .end_query = OpenGL::end_query,
            .get_query_results = OpenGL::get_query_results,
            .begin_debug_marker = OpenGL::begin_debug_marker,
            .end_debug_marker = OpenGL::end_debug_marker,
            .insert_debug_marker = OpenGL::insert_debug_marker,
            .create_pipeline_layout = OpenGL::create_pipeline_layout,
            .destroy_pipeline_layout = OpenGL::destroy_pipeline_layout,
            .create_compute_pipeline = OpenGL::create_compute_pipeline,
            .destroy_compute_pipeline = OpenGL::destroy_compute_pipeline,
            .create_mesh_pipeline = OpenGL::create_mesh_pipeline,
            .destroy_mesh_pipeline = OpenGL::destroy_mesh_pipeline,
            .create_texture_view = OpenGL::create_texture_view,
            .destroy_texture_view = OpenGL::destroy_texture_view,
            .begin_render_pass = OpenGL::begin_render_pass,
            .end_render_pass = OpenGL::end_render_pass
        };
    } // namespace Graphics
} // namespace Bcg
