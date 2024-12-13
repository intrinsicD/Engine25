//
// Created by alex on 13.12.24.
//

#ifndef ENGINE25_RENDERGRAPH_H
#define ENGINE25_RENDERGRAPH_H

#include "Dag.h"
#include "RenderPassData.h"

namespace Bcg::Graphics {
    class RenderGraph : public Dag<RenderPassData> {
    public:
        ~RenderGraph() override = default;

        std::vector<CommandBufferHandle> Execute(Device &device) {
            std::vector<CommandBufferHandle> command_buffers;

            // Topologically sort the graph to determine the order of passes
            auto order = TopologicalSort();

            // For each pass in sorted order, create and record into a command buffer
            for (auto node_id: order) {
                auto &pass_data = GetNode(node_id).data;

                // Create a command buffer for this pass
                CommandBufferDesc cmd_desc;
                cmd_desc.is_primary = true;
                cmd_desc.is_graphics = true;
                cmd_desc.is_compute = false;
                cmd_desc.allow_simultaneous_use = false;

                CommandBufferHandle cmd = device.CreateCommandBuffer(cmd_desc);

                // Begin recording commands
                device.BeginCommandBuffer(cmd);

                // Let the pass record its commands into the command buffer
                pass_data.RecordCommands(device, cmd);

                // End command buffer recording
                device.EndCommandBuffer(cmd);

                // Store the command buffer for submission
                command_buffers.push_back(cmd);
            }

            return command_buffers;
        }
    };
}

#endif //ENGINE25_RENDERGRAPH_H
