//
// Created by alex on 04.12.24.
//

#include <mutex>
#include "MainLoop.h"
#include "Engine.h"
#include "EngineEvents.h"
#include "RenderGraph.h"
#include "RenderGraph.h"

namespace Bcg {
    static std::mutex mutex;
    static bool running = false;

    void MainLoop::Run() {
        {
            std::scoped_lock lock(mutex);
            running = true;
        }

        while (running) {
            Engine::GetDispatcher().trigger<Events::Synchronize>();
            begin.Handle();
            prepare_scene.Handle();
            end_scene.Handle();
            prepare_render.Handle();
            end_render.Handle();
            render_gui.Handle();
            end_gui.Handle();
            end.Handle();
        }
    }

    void MainLoop::Run2() {
        {
            std::scoped_lock lock(mutex);
            running = true;
        }

        auto &device = Engine::GetContext().get<Graphics::Device>();
        auto &taskGraph = Engine::GetTaskGraph();
        auto &renderGraph = Engine::GetRenderGraph();
        auto &swapchainHandle = Engine::GetContext().get<Graphics::SwapchainHandle>();

        // Create semaphores and fence
        Graphics::SemaphoreHandle image_available_semaphore = device.create_semaphore();
        Graphics::SemaphoreHandle render_finished_semaphore = device.create_semaphore();
        Graphics::FenceHandle frame_fence = device.create_fence();

        while (running) {
            taskGraph.Clear();
            renderGraph.Clear();

            Engine::GetDispatcher().trigger<Events::BeginFrameEvent>();
            Engine::GetDispatcher().trigger<Events::UpdateSystems>();

            // Execute tasks
            taskGraph.ExecuteFullParallelUsingJobSystem();

            // Build and record render commands (assuming renderGraph.Execute(device) returns command buffers or sets them up)
            // If not, you need to modify it to return cmdBuffers or store them somewhere accessible.
            std::vector<Graphics::CommandBufferHandle> cmdBuffers = renderGraph.Execute(device);

            // Acquire next image from the swapchain
            Graphics::Uint32 image_index = 0;
            device.acquire_next_image(swapchainHandle, UINT64_MAX, image_available_semaphore, frame_fence, &image_index);

            // Now we have an image index and command buffers recorded. Submit them:
            Graphics::SubmitInfo submit_info;
            // Wait on the image_available_semaphore to ensure the swapchain image is ready to be rendered into.
            submit_info.wait_semaphores.push_back(image_available_semaphore);
            submit_info.command_buffers = cmdBuffers;
            // Signal render_finished_semaphore when done rendering
            submit_info.signal_semaphores.push_back(render_finished_semaphore);

            device.submit(submit_info);
            // Optionally wait for fences if you need CPU/GPU sync

            // Present the rendered image, waiting on render_finished_semaphore so the GPU is done rendering
            Graphics::PresentInfo present_info;
            present_info.swapchain = swapchainHandle;
            present_info.image_index = image_index;
            present_info.wait_semaphores.push_back(render_finished_semaphore);
            device.present(present_info);

            Engine::GetDispatcher().trigger<Events::EndFrameEvent>();
        }

        // Destroy semaphores and fence if needed before exit
        device.destroy_semaphore(image_available_semaphore);
        device.destroy_semaphore(render_finished_semaphore);
        device.destroy_fence(frame_fence);
    }


    void MainLoop::Stop() {
        std::scoped_lock lock(mutex);
        running = false;
    }
}