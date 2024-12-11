//
// Created by alex on 12/12/24.
//

#ifndef DEVICE_H
#define DEVICE_H

namespace Bcg::Graphics {
    class IGraphicsDevice {
    public:
        virtual ~IGraphicsDevice() = default;

        // Resource creation methods
        virtual IBuffer *createBuffer(const BufferDesc &desc) = 0;

        virtual ITexture *createTexture(const TextureDesc &desc) = 0;

        virtual ISampler *createSampler(const SamplerDesc &desc) = 0;

        virtual IShader *createShader(const ShaderDesc &desc) = 0;

        virtual IPipeline *createPipeline(const PipelineDesc &desc) = 0;

        // Command buffer creation
        virtual ICommandBuffer *createCommandBuffer(const CommandBufferDesc &desc) = 0;

        // Swapchain creation
        virtual ISwapchain *createSwapchain(const SwapchainDesc &desc) = 0;

        // Device capabilities
        virtual bool isFeatureSupported(DeviceFeature feature) const = 0;
    };
}
#endif //DEVICE_H
