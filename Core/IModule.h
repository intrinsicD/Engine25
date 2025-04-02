//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_IMODULE_H
#define ENGINE25_IMODULE_H

#include "Events/EngineEvents.h"
#include "IRenderer.h"
#include "UIManager.h"
#include "InputManager.h"

namespace Bcg{
    struct CurrentContext{
        entt::registry *scene;
        entt::dispatcher *dispatcher;

        IRenderer *renderer;
        UIManager *ui_manager;
        InputManager *input_manager;
    };

    class IModule{
    public:
        IModule() = default;

        virtual ~IModule() = default;

        virtual const char *getName() const = 0;

        virtual const char *getVersion() const = 0;

        virtual bool initialize(CurrentContext &context) = 0;

        virtual void shutdown() = 0;

        virtual void update(float deltaTime) = 0;

        virtual void render() = 0;

        virtual void renderUI() = 0;

    private:
        CurrentContext m_context;
    };
}

#endif //ENGINE25_IMODULE_H
