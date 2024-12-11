//
// Created by alex on 12/12/24.
//

#ifndef ISTATEOBJECTS_H
#define ISTATEOBJECTS_H

namespace Bcg::Graphics {
    struct RasterizationStateDesc {
        bool depthClampEnable;
        PolygonMode fillMode;
        CullMode cullMode;
        FrontFace frontFace;
        // Other properties
    };

    struct DepthStencilStateDesc {
        bool depthTestEnable;
        bool depthWriteEnable;
        CompareOp depthCompareOp;
        // Other properties
    };

    struct BlendStateDesc {
        bool blendEnable;
        BlendFactor srcColorBlendFactor;
        BlendFactor dstColorBlendFactor;
        BlendOp colorBlendOp;
        // Other properties
    };
}
#endif //ISTATEOBJECTS_H
