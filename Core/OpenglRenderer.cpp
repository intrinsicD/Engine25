//
// Created by alex on 02.04.25.
//

#include "OpenglRenderer.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Logger.h"
#include "entt/entity/registry.hpp"

namespace Bcg{
    void OpenglRenderer::initialize(GLFWwindow *window) {
        m_window = window;
        int version = gladLoadGL(glfwGetProcAddress);
        if(version == 0) {
            glfwDestroyWindow(m_window);
            glfwTerminate();
            LOG_FATAL("Application: Failed to initialize OpenGL context");
        }
        int major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        LOG_INFO(fmt::format("Application: OpenGL context initialized, version {}.{}", major, minor));
    }

    void OpenglRenderer::shutdown() {

    }

    void OpenglRenderer::render(entt::registry &scene) {
        int display_w, display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenglRenderer::setClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
}