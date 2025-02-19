//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_ENGINE_H
#define ENGINE25_ENGINE_H

#include "entt/entity/registry.hpp"
#include "entt/signal/dispatcher.hpp"
#include "RenderGraph.h"
#include "TaskGraph.h"

namespace Bcg {
    using Scene = entt::registry;
    using Context = entt::registry::context;
    using Dispatcher = entt::dispatcher;

    class Engine {
    public:
        Engine() = default;

        void init();

        void startup();

        void run();

        void shutdown();

        static Scene &get_scene();

        static Context &get_context();

        static Dispatcher &get_dispatcher();

        static TaskGraph &get_task_graph();

        static Graphics::RenderGraph &get_render_graph();
    private:
        const char *name = "Engine";
        const char *version = "0.0.1";
    };
}

#endif //ENGINE25_ENGINE_H
