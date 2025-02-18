//
// Created by alex on 18.02.25.
//

#include "OpenGLBackend.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Logger.h"

namespace Bcg::Graphics {
    static int gl_version__ = -1;

    //------------------------------------------------------------------------------------------------------------------
    //Create all the functions that are needed for the OpenGL backend
    //------------------------------------------------------------------------------------------------------------------

    //Get the OpenGL functions
    DeviceFunctions GetOpenGLDeviceFunctions() {
        DeviceFunctions funcs;

        return funcs;
    }

    OpenGLContext CreateOpenGLContext(const char *title, int width, int height) {
        if (!glfwInit()) {
            return {};
        }

        int major_version = 4;
        int minor_version = 5;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

        GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            LOG_FATAL("CreateOpenGLContext: Failed to create glfw window");
        }

        glfwMakeContextCurrent(window);

        if(gl_version__ < 0){
            gl_version__ = gladLoadGL(glfwGetProcAddress);
            if (!gl_version__) {
                glfwDestroyWindow(window);
                glfwTerminate();
                LOG_FATAL("CreateOpenGLContext: Failed to initialize OpenGL context");
            }
        }

        OpenGLContext context;
        context.window_handle = window;
        context.gl_context = nullptr;
        context.major_version = major_version;
        context.minor_version = minor_version;

        return context;
    }

    Device GetOpenGLDevice(OpenGLContext *backend_context) {
        return {GetOpenGLDeviceFunctions(), backend_context, BackendType::OpenGL};
    }
}