//
// Created by alex on 12/12/24.
//

#ifndef ITEXTURE_H
#define ITEXTURE_H

namespace Bcg::Graphics {
    enum class TextureType {
        Texture2D,
        Texture3D,
        TextureCube,
        // Add more as needed
    };

    struct TextureDesc {
        TextureType type;
        Format format;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t mipLevels;
        uint32_t arrayLayers;
        TextureUsage usage;
    };

    class ITexture {
    public:
        virtual ~ITexture() = default;
    };

    struct SamplerDesc {
        Filter minFilter;
        Filter magFilter;
        MipmapMode mipmapMode;
        AddressMode addressModeU;
        AddressMode addressModeV;
        AddressMode addressModeW;
        float mipLodBias;
        float maxAnisotropy;
        BorderColor borderColor;
    };

    class ISampler {
    public:
        virtual ~ISampler() = default;
    };

}
#endif //ITEXTURE_H
