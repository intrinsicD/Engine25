//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_GLFWWINDOW_H
#define ENGINE25_GLFWWINDOW_H

struct GLFWwindow;

namespace Bcg {
    struct Window {
        GLFWwindow *handle = nullptr;
        int width = 0;
        int height = 0;
        const char *title = nullptr;
        float clear_color[3];
    };
}

#endif //ENGINE25_GLFWWINDOW_H
