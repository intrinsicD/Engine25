//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_GLFWWINDOW_H
#define ENGINE25_GLFWWINDOW_H

#include <string>
#include "Math.h"

struct GLFWwindow;

namespace Bcg {
    struct Window {
        GLFWwindow *handle = nullptr;
        int width = 0;
        int height = 0;
        std::string title;
        Vector<float, 3> clear_color = {0.0f, 0.0f, 0.0f};
    };
}

#endif //ENGINE25_GLFWWINDOW_H
