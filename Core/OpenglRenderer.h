//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_OPENGLRENDERER_H
#define ENGINE25_OPENGLRENDERER_H

#include "IRenderer.h"

namespace Bcg{
    class OpenglRenderer : public IRenderer{
    public:
        OpenglRenderer() = default;

        ~OpenglRenderer() override = default;

        void initialize(GLFWwindow *window) override;

        void shutdown() override;

        void render(entt::registry &scene) override;

        void setClearColor(float r, float g, float b, float a) override;
    private:
        GLFWwindow *m_window = nullptr;
    };
}

#endif //ENGINE25_OPENGLRENDERER_H
