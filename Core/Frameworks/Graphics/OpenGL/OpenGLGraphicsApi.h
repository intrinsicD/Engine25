//
// Created by alex on 12.12.24.
//

#ifndef ENGINE25_OPENGLGRAPHICSAPI_H
#define ENGINE25_OPENGLGRAPHICSAPI_H

#include "GraphicsApi.h"
#include <map>
#include <mutex>

namespace Bcg::Graphics {
    class OpenGLMappings {
    public:
        static GLint ToGLShaderType(ShaderStage stage) {
            switch (stage) {
                case ShaderStage::Vertex:
                    return GL_VERTEX_SHADER;
                case ShaderStage::Fragment:
                    return GL_FRAGMENT_SHADER;
                case ShaderStage::Compute:
                    return GL_COMPUTE_SHADER;
                case ShaderStage::Geometry:
                    return GL_GEOMETRY_SHADER;
                case ShaderStage::TessellationControl:
                    return GL_TESS_CONTROL_SHADER;
                case ShaderStage::TessellationEvaluation:
                    return GL_TESS_EVALUATION_SHADER;
                default:
                    throw std::runtime_error("Unsupported shader stage.");
            }
        }

        static GLenum ToGLBufferUsage(BufferUsage usage) {
            switch (usage) {
                case BufferUsage::VertexBuffer:
                    return GL_ARRAY_BUFFER;
                case BufferUsage::IndexBuffer:
                    return GL_ELEMENT_ARRAY_BUFFER;
                case BufferUsage::UniformBuffer:
                    return GL_UNIFORM_BUFFER;
                case BufferUsage::StorageBuffer:
                    return GL_SHADER_STORAGE_BUFFER;
                default:
                    throw std::runtime_error("Unsupported buffer usage.");
            }
        }

        // Format mappings
        static GLint ToGLInternalFormat(Format format) {
            switch (format) {
                case Format::R8:
                    return GL_R8;
                case Format::R16:
                    return GL_R16;
                case Format::R32:
                    return GL_R32F;
                case Format::RG8:
                    return GL_RG8;
                case Format::RG16:
                    return GL_RG16;
                case Format::RG32:
                    return GL_RG32F;
                case Format::RGB8:
                    return GL_RGB8;
                case Format::RGB16:
                    return GL_RGB16;
                case Format::RGB32:
                    return GL_RGB32F;
                case Format::RGBA8:
                    return GL_RGBA8;
                case Format::RGBA16:
                    return GL_RGBA16;
                case Format::RGBA32:
                    return GL_RGBA32F;
                case Format::Depth32F:
                    return GL_DEPTH_COMPONENT32F;
                default:
                    throw std::runtime_error("Unsupported format.");
            }
        }

        static GLenum ToGLFormat(Format format) {
            switch (format) {
                case Format::R8:
                case Format::R16:
                case Format::R32:
                    return GL_RED;
                case Format::RG8:
                case Format::RG16:
                case Format::RG32:
                    return GL_RG;
                case Format::RGB8:
                case Format::RGB16:
                case Format::RGB32:
                    return GL_RGB;
                case Format::RGBA8:
                case Format::RGBA16:
                case Format::RGBA32:
                    return GL_RGBA;
                case Format::Depth32F:
                    return GL_DEPTH_COMPONENT;
                default:
                    throw std::runtime_error("Unsupported format.");
            }
        }

        static GLenum ToGLType(Format format) {
            switch (format) {
                case Format::R8:
                case Format::RG8:
                case Format::RGB8:
                case Format::RGBA8:
                    return GL_UNSIGNED_BYTE;
                case Format::R16:
                case Format::RG16:
                case Format::RGB16:
                case Format::RGBA16:
                    return GL_UNSIGNED_SHORT;
                case Format::R32:
                case Format::RG32:
                case Format::RGB32:
                case Format::RGBA32:
                    return GL_FLOAT;
                case Format::Depth32F:
                    return GL_FLOAT;
                default:
                    throw std::runtime_error("Unsupported format.");
            }
        }

        // CullMode mappings
        static GLenum ToGLCullMode(CullMode cullMode) {
            switch (cullMode) {
                case CullMode::None:
                    return GL_NONE;
                case CullMode::Front:
                    return GL_FRONT;
                case CullMode::Back:
                    return GL_BACK;
                case CullMode::FrontAndBack:
                    return GL_FRONT_AND_BACK;
                default:
                    throw std::runtime_error("Unsupported cull mode.");
            }
        }

        // PolygonMode mappings
        static GLenum ToGLPolygonMode(PolygonMode mode) {
            switch (mode) {
                case PolygonMode::Fill:
                    return GL_FILL;
                case PolygonMode::Line:
                    return GL_LINE;
                case PolygonMode::Point:
                    return GL_POINT;
                default:
                    throw std::runtime_error("Unsupported polygon mode.");
            }
        }

        // ShaderStage mappings
        static GLenum ToGLShaderStage(ShaderStage stage) {
            switch (stage) {
                case ShaderStage::Vertex:
                    return GL_VERTEX_SHADER;
                case ShaderStage::Fragment:
                    return GL_FRAGMENT_SHADER;
                case ShaderStage::Compute:
                    return GL_COMPUTE_SHADER;
                case ShaderStage::Geometry:
                    return GL_GEOMETRY_SHADER;
                case ShaderStage::TessellationControl:
                    return GL_TESS_CONTROL_SHADER;
                case ShaderStage::TessellationEvaluation:
                    return GL_TESS_EVALUATION_SHADER;
                default:
                    throw std::runtime_error("Unsupported shader stage.");
            }
        }

        // Filter mappings
        static GLint ToGLFilter(Filter filter) {
            switch (filter) {
                case Filter::Nearest:
                    return GL_NEAREST;
                case Filter::Linear:
                    return GL_LINEAR;
                default:
                    throw std::runtime_error("Unsupported filter.");
            }
        }

        // MipmapMode mappings
        static GLint ToGLMipmapMode(MipmapMode mode) {
            switch (mode) {
                case MipmapMode::Nearest:
                    return GL_NEAREST_MIPMAP_NEAREST;
                case MipmapMode::Linear:
                    return GL_LINEAR_MIPMAP_LINEAR;
                default:
                    throw std::runtime_error("Unsupported mipmap mode.");
            }
        }

        // AddressMode mappings
        static GLint ToGLWrapMode(AddressMode mode) {
            switch (mode) {
                case AddressMode::Repeat:
                    return GL_REPEAT;
                case AddressMode::MirroredRepeat:
                    return GL_MIRRORED_REPEAT;
                case AddressMode::ClampToEdge:
                    return GL_CLAMP_TO_EDGE;
                case AddressMode::ClampToBorder:
                    return GL_CLAMP_TO_BORDER;
                default:
                    throw std::runtime_error("Unsupported address mode.");
            }
        }

        // CompareOp mappings
        static GLenum ToGLCompareFunc(CompareOp op) {
            switch (op) {
                case CompareOp::Never:
                    return GL_NEVER;
                case CompareOp::Less:
                    return GL_LESS;
                case CompareOp::Equal:
                    return GL_EQUAL;
                case CompareOp::LessOrEqual:
                    return GL_LEQUAL;
                case CompareOp::Greater:
                    return GL_GREATER;
                case CompareOp::NotEqual:
                    return GL_NOTEQUAL;
                case CompareOp::GreaterOrEqual:
                    return GL_GEQUAL;
                case CompareOp::Always:
                    return GL_ALWAYS;
                default:
                    throw std::runtime_error("Unsupported compare operation.");
            }
        }

        // StencilOp mappings
        static GLenum ToGLStencilOp(StencilOp op) {
            switch (op) {
                case StencilOp::Keep:
                    return GL_KEEP;
                case StencilOp::Zero:
                    return GL_ZERO;
                case StencilOp::Replace:
                    return GL_REPLACE;
                case StencilOp::IncrementClamp:
                    return GL_INCR;
                case StencilOp::DecrementClamp:
                    return GL_DECR;
                case StencilOp::Invert:
                    return GL_INVERT;
                case StencilOp::IncrementWrap:
                    return GL_INCR_WRAP;
                case StencilOp::DecrementWrap:
                    return GL_DECR_WRAP;
                default:
                    throw std::runtime_error("Unsupported stencil operation.");
            }
        }

        // BlendFactor mappings
        static GLenum ToGLBlendFactor(BlendFactor factor) {
            switch (factor) {
                case BlendFactor::Zero:
                    return GL_ZERO;
                case BlendFactor::One:
                    return GL_ONE;
                case BlendFactor::SrcColor:
                    return GL_SRC_COLOR;
                case BlendFactor::OneMinusSrcColor:
                    return GL_ONE_MINUS_SRC_COLOR;
                case BlendFactor::SrcAlpha:
                    return GL_SRC_ALPHA;
                case BlendFactor::OneMinusSrcAlpha:
                    return GL_ONE_MINUS_SRC_ALPHA;
                case BlendFactor::DstColor:
                    return GL_DST_COLOR;
                case BlendFactor::OneMinusDstColor:
                    return GL_ONE_MINUS_DST_COLOR;
                case BlendFactor::DstAlpha:
                    return GL_DST_ALPHA;
                case BlendFactor::OneMinusDstAlpha:
                    return GL_ONE_MINUS_DST_ALPHA;
                default:
                    throw std::runtime_error("Unsupported blend factor.");
            }
        }

        // BlendOp mappings
        static GLenum ToGLBlendOp(BlendOp op) {
            switch (op) {
                case BlendOp::Add:
                    return GL_FUNC_ADD;
                case BlendOp::Subtract:
                    return GL_FUNC_SUBTRACT;
                case BlendOp::ReverseSubtract:
                    return GL_FUNC_REVERSE_SUBTRACT;
                case BlendOp::Min:
                    return GL_MIN;
                case BlendOp::Max:
                    return GL_MAX;
                default:
                    throw std::runtime_error("Unsupported blend operation.");
            }
        }

        // AddressMode mappings
        static GLint ToGLAddressMode(AddressMode mode) {
            switch (mode) {
                case AddressMode::Repeat:
                    return GL_REPEAT;
                case AddressMode::MirroredRepeat:
                    return GL_MIRRORED_REPEAT;
                case AddressMode::ClampToEdge:
                    return GL_CLAMP_TO_EDGE;
                case AddressMode::ClampToBorder:
                    return GL_CLAMP_TO_BORDER;
                default:
                    throw std::runtime_error("Unsupported address mode.");
            }
        }
    };

    class OpenGLResource : public IResource {
    protected:
        std::string debugName;
        ResourceType resourceType;

    public:
        OpenGLResource() = default;

        explicit OpenGLResource(ResourceType type) : resourceType(type) {}

        virtual ~OpenGLResource() = default;

        ResourceType GetResourceType() const override {
            return resourceType;
        }

        const char *GetDebugName() const override {
            return debugName.c_str();
        }

        void SetDebugName(const char *name) override {
            debugName = name ? name : "";
        }
    };

    class OpenGLBuffer : public OpenGLResource, public IBuffer {
    private:
        GLuint bufferId;
        BufferDesc desc;

    public:
        OpenGLBuffer() = default;

        explicit OpenGLBuffer(const BufferDesc &desc_) : desc(desc_), OpenGLResource(ResourceType::Buffer) {
            GLenum glUsage = OpenGLMappings::ToGLBufferUsage(desc.usage);

            glGenBuffers(1, &bufferId);
            glBindBuffer(glUsage, bufferId);
            glBufferData(glUsage, desc.size, nullptr, GL_STATIC_DRAW);
            glBindBuffer(glUsage, 0);
        }

        ~OpenGLBuffer() override {
            glDeleteBuffers(1, &bufferId);
        }

        GLuint GetBufferId() const {
            return bufferId;
        }

        void *Map() override {
            glBindBuffer(GL_ARRAY_BUFFER, bufferId);
            return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        }

        void Unmap() override {
            glBindBuffer(GL_ARRAY_BUFFER, bufferId);
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        const BufferDesc &GetDescription() const {
            return desc;
        }

        size_t GetSize() const override {
            return desc.size;
        }

        ResourceType GetResourceType() const override {
            return ResourceType::Buffer;
        }

        const char *GetDebugName() const override {
            return debugName.c_str();
        }

        void SetDebugName(const char *name) override {
            debugName = name ? name : "";
        }
    };

    class OpenGLTexture : public ITexture {
    private:
        GLuint textureId = 0;
        TextureDesc desc;
        const char *debugName = nullptr;

    public:
        OpenGLTexture() = default;

        explicit OpenGLTexture(const TextureDesc &desc_) : desc(desc_) {
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            GLint glInternalFormat = OpenGLMappings::ToGLInternalFormat(desc.format);
            GLenum glFormat = OpenGLMappings::ToGLFormat(desc.format);
            GLenum glType = OpenGLMappings::ToGLType(desc.format);

            glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, desc.width, desc.height, 0, glFormat, glType, nullptr);

            // Set default texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLMappings::ToGLFilter(desc.minFilter));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLMappings::ToGLFilter(desc.magFilter));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLMappings::ToGLAddressMode(desc.addressModeU));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLMappings::ToGLAddressMode(desc.addressModeV));

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        ~OpenGLTexture() override {
            if (textureId != 0) {
                glDeleteTextures(1, &textureId);
            }
        }

        const TextureDesc &GetDescription() const { return desc; }

        GLuint GetTextureId() const { return textureId; }

        const char *GetDebugName() const override { return debugName; }

        ResourceType GetResourceType() const override { return ResourceType::Texture; }

        void SetDebugName(const char *name) override { debugName = name; }

        Uint32 GetWidth() const override { return desc.width; }

        Uint32 GetHeight() const override { return desc.height; }

        Uint32 GetDepth() const override { return desc.depth; }

        Format GetFormat() const override { return desc.format; }

        Uint32 GetMipLevels() const override { return desc.mipLevels; }
    };

    class OpenGLSampler : public ISampler {
    private:
        GLuint samplerId;
        SamplerDesc desc;

    public:
        OpenGLSampler() = default;

        explicit OpenGLSampler(const SamplerDesc &desc_) : desc(desc_) {
            glGenSamplers(1, &samplerId);
            glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER,
                                desc.minFilter == Filter::Linear ? GL_LINEAR : GL_NEAREST);
            glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER,
                                desc.magFilter == Filter::Linear ? GL_LINEAR : GL_NEAREST);
            glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        ~OpenGLSampler() override {
            glDeleteSamplers(1, &samplerId);
        }

        GLuint GetSamplerId() const {
            return samplerId;
        }

        const SamplerDesc &GetDescription() const {
            return desc;
        }
    };

    class OpenGLShader : public IShader {
    private:
        GLuint shaderId = 0;
        ShaderDesc desc;
        std::string debugName;

    public:
        OpenGLShader() = default;

        explicit OpenGLShader(const ShaderDesc &desc_) : desc(desc_) {
            // Determine OpenGL shader type from ShaderStage
            GLenum shaderType = OpenGLMappings::ToGLShaderType(desc.stage);
            shaderId = glCreateShader(shaderType);
            if (shaderId == 0) {
                throw std::runtime_error("Failed to create OpenGL shader.");
            }

            // Apply preprocessor macros if provided
            std::string modifiedCode = ApplyMacros(desc.code, desc.macros);

            // Set shader source and compile
            const char *source = modifiedCode.c_str();
            glShaderSource(shaderId, 1, &source, nullptr);
            glCompileShader(shaderId);

            // Check for compilation errors
            GLint success;
            glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
            if (!success) {
                char infoLog[1024];
                glGetShaderInfoLog(shaderId, sizeof(infoLog), nullptr, infoLog);
                glDeleteShader(shaderId);
                throw std::runtime_error(std::string("Shader compilation failed: ") + infoLog);
            }

            // Set debug name if provided
            if (desc.debugName) {
                debugName = desc.debugName;
                glObjectLabel(GL_SHADER, shaderId, -1, debugName.c_str());
            }
        }

        ~OpenGLShader() override {
            if (shaderId != 0) {
                glDeleteShader(shaderId);
            }
        }

        GLuint GetShaderId() const {
            return shaderId;
        }

        const ShaderDesc &GetDescription() const {
            return desc;
        }

        const char *GetDebugName() const override {
            return debugName.c_str();
        }

    private:
        // Applies preprocessor macros to the shader source code
        static std::string ApplyMacros(const char *code, const std::vector<std::string> &macros) {
            std::string modifiedCode;
            for (const auto &macro: macros) {
                modifiedCode += "#define " + macro + "\n";
            }
            modifiedCode += code;
            return modifiedCode;
        }
    };

    class OpenGLDescriptorSet : public IDescriptorSet {
    private:
        std::unordered_map<Uint32, DescriptorBinding> bindings;

    public:
        ~OpenGLDescriptorSet() override = default;

        void UpdateBindings(const std::vector<DescriptorBinding> &newBindings) override {
            for (const auto &binding: newBindings) {
                bindings[binding.binding] = binding;

                // Apply the binding to OpenGL
                switch (binding.type) {
                    case DescriptorType::UniformBuffer:
                        BindUniformBuffer(binding);
                        break;

                    case DescriptorType::SampledImage:
                    case DescriptorType::CombinedImageSampler:
                        BindTextures(binding);
                        break;

                    case DescriptorType::StorageBuffer:
                        BindStorageBuffer(binding);
                        break;

                    default:
                        throw std::runtime_error(
                                "Unsupported descriptor type: " + std::to_string(static_cast<int>(binding.type)));
                }
            }
        }

        void ResetBindings() override {
            for (const auto &[bindingIndex, binding]: bindings) {
                switch (binding.type) {
                    case DescriptorType::UniformBuffer:
                    case DescriptorType::StorageBuffer:
                        glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, 0);
                        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, 0);
                        break;

                    case DescriptorType::SampledImage:
                    case DescriptorType::CombinedImageSampler:
                        glActiveTexture(GL_TEXTURE0 + bindingIndex);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glBindSampler(bindingIndex, 0);
                        break;

                    default:
                        break; // No action for unsupported types
                }
            }
            bindings.clear();
        }

        const DescriptorBinding *GetBinding(Uint32 bindingIndex) const override {
            auto it = bindings.find(bindingIndex);
            return it != bindings.end() ? &it->second : nullptr;
        }

    private:
        void BindUniformBuffer(const DescriptorBinding &binding) {
            for (Uint32 i = 0; i < binding.resources.size(); ++i) {
                auto *buffer = dynamic_cast<OpenGLBuffer *>(binding.resources[i].get());
                if (buffer) {
                    glBindBufferBase(GL_UNIFORM_BUFFER, binding.binding + i, buffer->GetBufferId());
                } else {
                    throw std::runtime_error(
                            "Invalid UniformBuffer resource at binding: " + std::to_string(binding.binding));
                }
            }
        }

        void BindTextures(const DescriptorBinding &binding) {
            for (Uint32 i = 0; i < binding.resources.size(); ++i) {
                auto *texture = dynamic_cast<OpenGLTexture *>(binding.resources[i].get());
                if (texture) {
                    glActiveTexture(GL_TEXTURE0 + binding.binding + i);
                    glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
                    auto *sampler = dynamic_cast<OpenGLSampler *>(binding.resources[i].get());
                    if (sampler) {
                        glBindSampler(binding.binding + i, sampler->GetSamplerId());
                    }
                } else {
                    throw std::runtime_error("Invalid Texture resource at binding: " + std::to_string(binding.binding));
                }
            }
        }

        void BindStorageBuffer(const DescriptorBinding &binding) {
            for (Uint32 i = 0; i < binding.resources.size(); ++i) {
                auto *buffer = dynamic_cast<OpenGLBuffer *>(binding.resources[i].get());
                if (buffer) {
                    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding.binding + i, buffer->GetBufferId());
                } else {
                    throw std::runtime_error(
                            "Invalid StorageBuffer resource at binding: " + std::to_string(binding.binding));
                }
            }
        }
    };

    class OpenGLCommandBuffer : public ICommandBuffer {
    private:
        bool isRecording;
        CommandBufferDesc desc;

    public:
        OpenGLCommandBuffer() = default;

        OpenGLCommandBuffer(const CommandBufferDesc &desc_) : desc(desc_), isRecording(false) {
            // In OpenGL, most state and commands are issued directly.
        }

        const CommandBufferDesc &GetDescription() const {
            return desc;
        }

        void Begin() override {
            isRecording = true;
            // In OpenGL, most state and commands are issued directly.
        }

        void End() override {
            isRecording = false;
        }

        void Submit() override {
            // OpenGL executes commands immediately; submission is implicit.
        }

        void Reset() override {
            // Reset command buffer state if necessary.
        }

        bool IsRecording() const override {
            return isRecording;
        }
    };

    class OpenGLSemaphore : public ISemaphore {
    private:
        GLsync syncObject = nullptr;

    public:
        ~OpenGLSemaphore() override {
            if (syncObject) {
                glDeleteSync(syncObject);
            }
        }

        // Wait for the semaphore to be signaled
        void Wait(Uint64 timeout = UINT64_MAX) override {
            if (!syncObject) {
                throw std::runtime_error("Semaphore has not been signaled.");
            }

            GLenum result = glClientWaitSync(syncObject, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
            if (result == GL_WAIT_FAILED) {
                throw std::runtime_error("Failed to wait for semaphore.");
            }
        }

        // Check if the semaphore is signaled
        bool IsSignaled() const override {
            if (!syncObject) {
                return false;
            }

            GLint status = 0;
            glGetSynciv(syncObject, GL_SYNC_STATUS, sizeof(status), nullptr, &status);
            return status == GL_SIGNALED;
        }

        // Signal the semaphore manually (useful for CPU-GPU sync)
        void Signal() override {
            if (syncObject) {
                glDeleteSync(syncObject); // Delete existing sync object
            }
            syncObject = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            if (!syncObject) {
                throw std::runtime_error("Failed to create semaphore sync object.");
            }
        }
    };


    class OpenGLTimelineSemaphore : public ITimelineSemaphore {
    private:
        mutable std::mutex mutex;                          // Protects access to timeline values
        mutable Uint64 currentValue = 0;                   // The current value of the timeline semaphore
        mutable std::map<Uint64, GLsync> timelineSyncs;    // Maps timeline values to OpenGL sync objects

    public:
        ~OpenGLTimelineSemaphore() override {
            std::lock_guard<std::mutex> lock(mutex);
            for (auto &[value, sync]: timelineSyncs) {
                glDeleteSync(sync);
            }
            timelineSyncs.clear();
        }

        // Wait for the semaphore to reach or exceed the specified value
        void WaitForValue(Uint64 value, Uint64 timeout = UINT64_MAX) override {
            std::lock_guard<std::mutex> lock(mutex);

            auto it = timelineSyncs.find(value);
            if (it == timelineSyncs.end()) {
                throw std::runtime_error("Requested timeline value not found.");
            }

            GLenum result = glClientWaitSync(it->second, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
            if (result == GL_WAIT_FAILED) {
                throw std::runtime_error("Failed to wait for timeline semaphore value.");
            }

            // Update current value after wait
            if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
                currentValue = std::max(currentValue, value);
            }
        }

        // Signal the semaphore with the specified value
        void SignalValue(Uint64 value) override {
            std::lock_guard<std::mutex> lock(mutex);

            // Ensure the value is increasing
            if (value <= currentValue) {
                throw std::runtime_error("Timeline semaphore value must be strictly increasing.");
            }

            // Create a new sync object for this value
            GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            if (!sync) {
                throw std::runtime_error("Failed to create timeline semaphore sync object.");
            }

            timelineSyncs[value] = sync;
        }

        // Get the current value of the semaphore
        Uint64 GetCurrentValue() const override {
            std::lock_guard<std::mutex> lock(mutex);

            // Check all sync objects and update the current value
            for (auto it = timelineSyncs.begin(); it != timelineSyncs.end();) {
                GLint result = 0;
                glGetSynciv(it->second, GL_SYNC_STATUS, sizeof(result), nullptr, &result);
                if (result == GL_SIGNALED) {
                    currentValue = std::max(currentValue, it->first);
                    glDeleteSync(it->second);
                    it = timelineSyncs.erase(it);
                } else {
                    ++it;
                }
            }

            return currentValue;
        }

        // Wait for the semaphore (for backward compatibility with ISemaphore)
        void Wait(Uint64 timeout = UINT64_MAX) override {
            WaitForValue(currentValue + 1, timeout);
        }

        // Check if the semaphore is signaled
        bool IsSignaled() const override {
            return GetCurrentValue() >= currentValue + 1;
        }

        // Signal the semaphore (for backward compatibility with ISemaphore)
        void Signal() override {
            SignalValue(currentValue + 1);
        }
    };

    class OpenGLFence : public IFence {
    private:
        GLsync syncObject = nullptr;

    public:
        ~OpenGLFence() override {
            if (syncObject) {
                glDeleteSync(syncObject);
            }
        }

        // Wait until the fence is signaled or timeout expires
        void Wait(Uint64 timeout = UINT64_MAX) override {
            if (!syncObject) {
                throw std::runtime_error("Fence is not initialized or has been reset.");
            }

            GLenum result = glClientWaitSync(syncObject, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
            if (result == GL_WAIT_FAILED) {
                throw std::runtime_error("Failed to wait for fence.");
            }
        }

        // Check if the fence is signaled without blocking
        bool IsSignaled() const override {
            if (!syncObject) {
                return false; // Fence is not initialized or reset
            }

            GLint status = 0;
            glGetSynciv(syncObject, GL_SYNC_STATUS, sizeof(status), nullptr, &status);
            return status == GL_SIGNALED;
        }

        // Reset the fence to an unsignaled state
        void Reset() override {
            if (syncObject) {
                glDeleteSync(syncObject);
                syncObject = nullptr;
            }
        }

        // Signal the fence by inserting it into the OpenGL command stream
        void Signal() {
            Reset(); // Ensure any previous sync object is cleaned up
            syncObject = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            if (!syncObject) {
                throw std::runtime_error("Failed to create fence sync object.");
            }
        }
    };

    class OpenGLSwapchain : public ISwapchain {
    private:
        SwapchainDesc desc;
        GLFWwindow *window;

        std::vector<ITexture *> backBuffers; // If custom framebuffers are used
        ITexture *currentImage = nullptr;  // Placeholder for current back buffer

    public:
        OpenGLSwapchain() = default;

        explicit OpenGLSwapchain(const SwapchainDesc &desc, GLFWwindow *glfwWindow) : desc(desc), window(glfwWindow) {
            if (!window) {
                throw std::runtime_error("Invalid GLFW window.");
            }

            // Optionally create custom framebuffers or textures
            CreateBackBuffers();

        }

        ~OpenGLSwapchain() override {
            for (auto *buffer: backBuffers) {
                delete buffer;
            }
            backBuffers.clear();
        }

        const SwapchainDesc &GetDescription() const { return desc; }

        void Present(ISemaphore *waitSemaphore = nullptr) override {
            // Optionally wait for the semaphore (CPU-GPU synchronization)
            if (waitSemaphore) {
                waitSemaphore->Wait();
            }

            // Present the back buffer
            glfwSwapBuffers(window);
        }

        ITexture *GetCurrentImage() override {
            return currentImage; // Default framebuffer doesn't expose textures directly
        }

        void Recreate(Uint32 newWidth, Uint32 newHeight, Format newFormat) override {
            desc.width = newWidth;
            desc.height = newHeight;
            desc.format = newFormat;

            // Recreate custom framebuffers or textures if necessary
            CreateBackBuffers();
        }

    private:
        void CreateBackBuffers() {
            for (auto *buffer: backBuffers) {
                delete buffer;
            }
            backBuffers.clear();

            // Example: Create a single custom texture as the back buffer
            if (desc.imageCount > 1) {
                for (Uint32 i = 0; i < desc.imageCount; ++i) {
                    TextureDesc textureDesc;
                    textureDesc.width = desc.width;
                    textureDesc.height = desc.height;
                    textureDesc.format = desc.format;
                    auto *texture = new OpenGLTexture(textureDesc); // Assuming OpenGLTexture implements ITexture
                    backBuffers.push_back(texture);
                }
            }

            currentImage = backBuffers.empty() ? nullptr : backBuffers[0];
        }
    };


    class OpenGLPipeline : public IPipeline {
    private:
        GLuint programId = 0;
        PipelineDesc pipelineDesc;
        std::unordered_map<ShaderStage, IShader *> shaders;

    public:
        OpenGLPipeline() = default;

        explicit OpenGLPipeline(PipelineDesc desc) : pipelineDesc(std::move(desc)) {
            programId = glCreateProgram();
            if (programId == 0) {
                throw std::runtime_error("Failed to create OpenGL shader program.");
            }

            AttachShaders();
            LinkProgram();
        }

        ~OpenGLPipeline() override {
            if (programId != 0) {
                glDeleteProgram(programId);
            }
        }

        const PipelineDesc &GetDescription() const override {
            return pipelineDesc;
        }

        IShader *GetShader(ShaderStage stage) const override {
            auto it = shaders.find(stage);
            return it != shaders.end() ? it->second : nullptr;
        }

        GLuint GetProgramId() const {
            return programId;
        }

    private:
        void AttachShaders() {
            // Iterate over shader stages in the pipeline description
            for (const auto &[stage, shader]: {
                    std::pair(ShaderStage::Vertex, pipelineDesc.shaders.vertexShader),
                    std::pair(ShaderStage::Fragment, pipelineDesc.shaders.fragmentShader),
                    std::pair(ShaderStage::Geometry, pipelineDesc.shaders.geometryShader),
                    std::pair(ShaderStage::TessellationControl, pipelineDesc.shaders.tessellationControlShader),
                    std::pair(ShaderStage::TessellationEvaluation, pipelineDesc.shaders.tessellationEvaluationShader),
                    std::pair(ShaderStage::Compute, pipelineDesc.shaders.computeShader)
            }) {
                if (shader) {
                    auto *oglShader = dynamic_cast<OpenGLShader *>(shader);
                    if (!oglShader) {
                        throw std::runtime_error("Invalid shader type provided for OpenGL pipeline.");
                    }
                    shaders[stage] = shader;
                    glAttachShader(programId, oglShader->GetShaderId());
                }
            }
        }

        void LinkProgram() {
            glLinkProgram(programId);

            GLint linkStatus;
            glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
            if (linkStatus == GL_FALSE) {
                GLint logLength;
                glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);

                std::string log(logLength, '\0');
                glGetProgramInfoLog(programId, logLength, nullptr, log.data());

                glDeleteProgram(programId);
                programId = 0;

                throw std::runtime_error("Failed to link OpenGL shader program: " + log);
            }
        }
    };

    class OpenGLRenderPass : public IRenderPass {
    private:
        RenderPassDesc renderPassDesc;
        GLuint framebuffer = 0;
        std::vector<GLuint> colorAttachments;
        GLuint depthStencilAttachment = 0;

    public:
        OpenGLRenderPass() = default;

        explicit OpenGLRenderPass(RenderPassDesc desc) : renderPassDesc(std::move(desc)) {
            glGenFramebuffers(1, &framebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            // Create and attach color attachments
            for (size_t i = 0; i < renderPassDesc.attachments.size(); ++i) {
                const auto &attachment = renderPassDesc.attachments[i];

                if (attachment.isDepthStencil) {
                    depthStencilAttachment = CreateDepthStencilAttachment(attachment);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthStencilAttachment,
                                           0);
                } else {
                    GLuint colorAttachment = CreateColorAttachment(attachment);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachment, 0);
                    colorAttachments.push_back(colorAttachment);
                }
            }

            // Check framebuffer completeness
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                throw std::runtime_error("Framebuffer is not complete.");
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        ~OpenGLRenderPass() override {
            glDeleteFramebuffers(1, &framebuffer);
            for (GLuint attachment: colorAttachments) {
                glDeleteTextures(1, &attachment);
            }
            if (depthStencilAttachment != 0) {
                glDeleteTextures(1, &depthStencilAttachment);
            }
        }

        const RenderPassDesc &GetDescription() const override {
            return renderPassDesc;
        }

        void Begin(ICommandBuffer *commandBuffer, const std::vector<ClearValue> &clearValues) override {
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

            // Clear color attachments
            for (size_t i = 0; i < colorAttachments.size(); ++i) {
                const auto &clearValue = clearValues[i];
                glClearColor(clearValue.color[0], clearValue.color[1], clearValue.color[2], clearValue.color[3]);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            // Clear depth/stencil attachment
            if (depthStencilAttachment != 0) {
                const auto &depthStencilClear = clearValues[colorAttachments.size()].depthStencil;
                glClearDepth(depthStencilClear.depth);
                glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            }
        }

        void End(ICommandBuffer *commandBuffer) override {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        bool IsCompatibleWith(const IPipeline *pipeline) const override {
            // Ensure the pipeline is compatible with the attachments
            const auto &pipelineDesc = pipeline->GetDescription();
            return renderPassDesc.attachments.size() == pipelineDesc.renderPass.attachments.size();
        }

    private:
        GLuint CreateColorAttachment(const RenderPassDesc::AttachmentDescription &attachment) {
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            GLint internalFormat = OpenGLMappings::ToGLInternalFormat(attachment.format);
            GLenum format = OpenGLMappings::ToGLFormat(attachment.format);
            GLenum type = OpenGLMappings::ToGLType(attachment.format);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 800, 600, 0, format, type,
                         nullptr); // Replace 800x600 with actual size
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            return texture;
        }

        GLuint CreateDepthStencilAttachment(const RenderPassDesc::AttachmentDescription &attachment) {
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            GLint internalFormat = OpenGLMappings::ToGLInternalFormat(attachment.format);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                         nullptr); // Replace 800x600 with actual size
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            return texture;
        }
    };


    class OpenGLDevice : public IDevice {
    public:
        IBuffer *CreateBuffer(const BufferDesc &desc) override {
            return new OpenGLBuffer(desc);
        }

        ITexture *CreateTexture(const TextureDesc &desc) override {
            return new OpenGLTexture(desc);
        }

        ISampler *CreateSampler(const SamplerDesc &desc) override {
            return new OpenGLSampler(desc);
        }

        IShader *CreateShader(const ShaderDesc &desc) override {
            return new OpenGLShader(desc.stage, desc.code, desc.entryPoint);
        }

        ICommandBuffer *CreateCommandBuffer(const CommandBufferDesc &desc) override {
            return new OpenGLCommandBuffer();
        }

        // Add other implementations for render passes, pipelines, etc.
    };
}

#endif //ENGINE25_OPENGLGRAPHICSAPI_H
