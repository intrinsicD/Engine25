//
// Created by alex on 12/12/24.
//

#ifndef ISHADER_H
#define ISHADER_H

namespace Bcg::Graphics {
    enum class ShaderStage {
        Vertex,
        Fragment,
        Compute,
        Geometry,
        TessellationControl,
        TessellationEvaluation,
        // Add more stages as needed
    };

    struct ShaderDesc {
        ShaderStage stage;
        const char *code; // Source code or binary data
        size_t codeSize;
        const char *entryPoint; // Optional entry point name
    };

    class IShader {
    public:
        virtual ~IShader() = default;
    };
}

#endif //ISHADER_H
