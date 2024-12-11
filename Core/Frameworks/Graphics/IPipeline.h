//
// Created by alex on 12/12/24.
//

#ifndef IPIPELINE_H
#define IPIPELINE_H

namespace Bcg::Graphics {
    struct PipelineDesc {
        IShader *vertexShader;
        IShader *fragmentShader;
        // Optional stages
        IShader *geometryShader;
        IShader *tessControlShader;
        IShader *tessEvalShader;
        // Input layout
        InputLayoutDesc inputLayout;
        // Render state
        RasterizationStateDesc rasterizationState;
        DepthStencilStateDesc depthStencilState;
        BlendStateDesc blendState;
        // Pipeline layout
        IPipelineLayout *pipelineLayout;
        // Render pass compatibility (for Vulkan)
        IRenderPass *renderPass;
    };

    class IPipeline {
    public:
        virtual ~IPipeline() = default;
    };
}

#endif //IPIPELINE_H
