//
// Created by alex on 13.12.24.
//

#ifndef ENGINE25_RENDERPASSDATA_H
#define ENGINE25_RENDERPASSDATA_H

#include "GraphicsApi.h"

namespace Bcg::Graphics{
    struct RenderPassData {
        Bcg::Graphics::RenderPassHandle render_pass;   // Created elsewhere
        Bcg::Graphics::FramebufferHandle framebuffer;  // Created elsewhere
        Bcg::Graphics::PipelineHandle pipeline;
        std::vector<Bcg::Graphics::DescriptorSetHandle> descriptor_sets;
        // Any other data needed to issue draw calls (vertex buffers, index buffers, etc.)

        void RecordCommands(Bcg::Graphics::Device &device, Bcg::Graphics::CommandBufferHandle cmd) {
            // Begin the render pass
            // device.BeginRenderPass(cmd, render_pass, framebuffer, clear_values, ...);

            // Bind pipeline
            // device.BindPipeline(cmd, pipeline);

            // Bind descriptor sets
            // for (auto ds : descriptor_sets) {
            //    device.BindDescriptorSet(cmd, ds);
            // }

            // Issue draw calls
            // device.Draw(cmd, vertex_count, instance_count, first_vertex, first_instance);

            // End the render pass
            // device.EndRenderPass(cmd);
        }
    };
}

#endif //ENGINE25_RENDERPASSDATA_H
