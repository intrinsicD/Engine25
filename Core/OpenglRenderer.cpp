//
// Created by alex on 02.04.25.
//

#include "OpenglRenderer.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Logger.h"
#include "entt/entity/registry.hpp"

namespace Bcg{
    OpenglRenderer::OpenglRenderer() : IRenderer("OpenGL Renderer", "0.1.0"){}

    bool OpenglRenderer::initialize(ApplicationContext *context) {
        if(!context){
            LOG_FATAL("OpenglRenderer: Context is null");
            return false;
        }

        if(IModule::initialize(context)){
            int version = gladLoadGL(glfwGetProcAddress);
            if(version == 0) {
                glfwDestroyWindow(context->windowPtr);
                glfwTerminate();
                LOG_FATAL("Application: Failed to initialize OpenGL context");
            }
            int major, minor;
            glGetIntegerv(GL_MAJOR_VERSION, &major);
            glGetIntegerv(GL_MINOR_VERSION, &minor);
            LOG_INFO(fmt::format("Application: OpenGL context initialized, version {}.{}", major, minor));

            assert(context->backend.opengl.majorVersion == major);
            assert(context->backend.opengl.minorVersion == minor);
            return true;
        }
        return false;
    }

    void OpenglRenderer::shutdown() {

    }

    void OpenglRenderer::render() {
        int display_w, display_h;
        glfwGetFramebufferSize(m_app_context->windowPtr, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenglRenderer::beginFrame() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenglRenderer::endFrame() {
        glfwSwapBuffers(m_app_context->windowPtr);
    }
}