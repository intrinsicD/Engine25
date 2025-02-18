//
// Created by alex on 18.02.25.
//

#ifndef ENGINE25_OPENGLBACKEND_H
#define ENGINE25_OPENGLBACKEND_H

#include "GraphicsApi.h"

namespace Bcg::Graphics {
    struct OpenGLContext {
        void *window_handle = nullptr;
        void *gl_context = nullptr;
        int minor_version = 0;
        int major_version = 0;
    };

    DeviceFunctions GetOpenGLDeviceFunctions();

    OpenGLContext CreateOpenGLContext(const char *title, int width, int height);

    Device GetOpenGLDevice(OpenGLContext *backend_context);
}


#endif //ENGINE25_OPENGLBACKEND_H
