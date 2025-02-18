//
// Created by alex on 18.02.25.
//

#ifndef ENGINE25_OPENGLBACKEND_H
#define ENGINE25_OPENGLBACKEND_H

#include "GLFW/glfw3.h"
#include "glad/gl.h"
#include "GraphicsApi.h"

namespace Bcg::Graphics {
    struct OpenGLContext {
        GLFWwindow* window;                // Native window handle (e.g., GLFW window).
        int width;                         // Current window width.
        int height;                        // Current window height.
        std::string version;               // OpenGL version string.
        std::string vendor;                // Vendor string.
        std::vector<std::string> extensions; // List of supported OpenGL extensions.

        // Debug output:
        // Flag indicating whether debug output is enabled.
        bool debugOutputEnabled = false;

        // Additional state caches:
        GLuint currentProgram = 0;         // Currently bound shader program.
        GLuint currentFramebuffer = 0;     // Currently bound framebuffer.
        GLuint currentVAO = 0;             // Currently bound Vertex Array Object.

        // Tracking currently bound textures across multiple texture units.
        static constexpr int MAX_TEXTURE_UNITS = 16;
        // An array to track the texture bound to each texture unit.
        GLuint boundTextures[MAX_TEXTURE_UNITS] = { 0 };

        // You can add more fields as needed, such as currently bound buffers (VBOs, EBOs),
        // renderbuffers, or any other state you wish to cache for performance or debugging.
    };

    void EnableDebugOutput(OpenGLContext* context);

    DeviceFunctions GetOpenGLDeviceFunctions();

    OpenGLContext CreateOpenGLContext(const char *title, int width, int height);

    Device GetOpenGLDevice(OpenGLContext *backend_context);
}


#endif //ENGINE25_OPENGLBACKEND_H
