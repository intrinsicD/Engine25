//
// Created by alex on 04.12.24.
//

#include <mutex>
#include "MainLoop.h"
#include "Engine.h"
#include "EngineEvents.h"

namespace Bcg {
    static std::mutex mutex;
    static bool running = false;

    void MainLoop::Run() {
        {
            std::scoped_lock lock(mutex);
            running = true;
        }

        while (running) {
            Engine::get_dispatcher().trigger<Events::Synchronize>();
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

    void MainLoop::Stop() {
        std::scoped_lock lock(mutex);
        running = false;
    }
}