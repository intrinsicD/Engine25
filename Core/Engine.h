//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_ENGINE_H
#define ENGINE25_ENGINE_H

#include "entt/entity/registry.hpp"
#include "entt/signal/dispatcher.hpp"

namespace Bcg {
    using Scene = entt::registry;
    using Context = entt::registry::context;
    using Dispatcher = entt::dispatcher;

    class Engine {
    public:
        Engine() = default;

        void Init();

        void Startup();

        void Run();

        void Shutdown();

        static Scene &GetScene();

        static Context &GetContext();

        static Dispatcher &GetDispatcher();
    private:
        const char *name = "Engine";
        const char *version = "0.0.1";
    };
}

#endif //ENGINE25_ENGINE_H
