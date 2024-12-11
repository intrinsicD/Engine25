//
// Created by alex on 12/12/24.
//

#ifndef ISWAPCHAIN_H
#define ISWAPCHAIN_H

namespace Bcg::Graphics {
    struct SwapchainDesc {
        uint32_t width;
        uint32_t height;
        Format format;
        PresentMode presentMode;
        uint32_t imageCount;
    };

    class ISwapchain {
    public:
        virtual ~ISwapchain() = default;

        virtual ITexture *getCurrentImage() = 0;

        virtual void present(ISemaphore *waitSemaphore = nullptr) = 0;
    };
}
#endif //ISWAPCHAIN_H
