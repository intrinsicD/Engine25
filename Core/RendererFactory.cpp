//
// Created by alex on 02.04.25.
//

#include "RendererFactory.h"
#include "Logger.h"
#include "OpenglRenderer.h"

namespace Bcg::RendererFactory {
    std::unique_ptr<IRenderer> CreateRenderer(BackendDesc &backendDesc) {
        LOG_INFO(fmt::format("RendererFactory: Attempting to create renderer of type '{}'", backendDesc.type));

        if (backendDesc.type == "OpenGL") {
            LOG_INFO("RendererFactory: OpenGL renderer created!");
            return std::make_unique<OpenglRenderer>();
        } else if (backendDesc.type == "Vulkan") {
            LOG_TODO("RendererFactory: Vulkan renderer is not implemented yet!");
            // else if (type == "vulkan" || type == "Vulkan") {
            //     return std::make_unique<VulkanRenderer>(); // Example
        }

        LOG_ERROR(fmt::format("RendererFactory: Unknown or unsupported renderer type requested!"));
        return nullptr;
    }
}