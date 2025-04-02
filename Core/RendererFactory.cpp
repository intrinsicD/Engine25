//
// Created by alex on 02.04.25.
//

#include "RendererFactory.h"
#include "Logger.h"
#include "OpenglRenderer.h"

namespace Bcg::RendererFactory {
    std::unique_ptr<IRenderer> CreateRenderer(BackendDescriptor &desc) {
        LOG_INFO(fmt::format("RendererFactory: Attempting to create renderer of type '{}'",
                             RendererTypeToString(desc.type)));

        if (desc.type == RendererType::OpenGL) {
            return std::make_unique<OpenglRenderer>();
        } else if (desc.type == RendererType::Vulkan) {
            // else if (type == "vulkan" || type == "Vulkan") {
            //     return std::make_unique<VulkanRenderer>(); // Example
        }

        LOG_ERROR(fmt::format("RendererFactory: Unknown or unsupported renderer type requested!"));
        return nullptr;
    }
}