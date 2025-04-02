//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_IRENDERER_H
#define ENGINE25_IRENDERER_H

#include "entt/entity/fwd.hpp"

struct GLFWwindow;

namespace Bcg{
    class IRenderer{
    public:
        IRenderer() = default;

        virtual ~IRenderer() = default;

        virtual void initialize(GLFWwindow *window) = 0;

        virtual void shutdown() = 0;

        virtual void render(entt::registry &scene) = 0;

        virtual void setClearColor(float r, float g, float b, float a) = 0;
    private:

    };
}

#endif //ENGINE25_IRENDERER_H
