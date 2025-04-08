//
// Created by alex on 04.04.25.
//

#ifndef ENGINE25_APPLICATIONCONTEXT_H
#define ENGINE25_APPLICATIONCONTEXT_H

struct GLFWwindow;

#include "entt/entity/fwd.hpp"
#include "entt/signal/fwd.hpp"

namespace Bcg {
    class UIManager;

    class VulkanRenderer;

    class ModuleManager;

    class Platform;

    class SlangShaderModule;

    struct BackendDesc {
        std::string type = "OpenGL";
        struct OpenGLDesc {
            int majorVersion = 4;
            int minorVersion = 6;
            int profile = 0; // 0 for Core, 1 for Compatibility
            bool debugContext = true;
        } opengl;

        struct VulkanDesc {
            int majorVersion = 1;
            int minorVersion = 2;
            bool debugContext = true;
        } vulkan;

        int vsync = 1; // 1 for enabled, 0 for disabled
    };

    struct WindowDesc {
        int width = 1280;
        int height = 720;
        std::string title = "Bcg Application";
        // Add vsync, fullscreen flags etc. if needed
    };

    struct MouseDesc {
        int x = 0;
        int y = 0;
        int scroll_x = 0;
        int scroll_y = 0;
        bool leftButtonPressed = false;
        bool middleButtonPressed = false;
        bool rightButtonPressed = false;
        bool moves = false;
    };

    struct KeyboardDesc {
        bool keys[1024] = {false};
        bool keyPressed = false;
        bool keyReleased = false;
    };

    struct ApplicationContext {
        GLFWwindow *windowPtr = nullptr;
        WindowDesc windowDesc;
        BackendDesc backend;
        MouseDesc mouse;
        KeyboardDesc keyboard;

        UIManager *uiManager;
        ModuleManager *moduleManager;
        VulkanRenderer *renderer;
        Platform *platform;
        SlangShaderModule *shaderModule;

        entt::dispatcher *dispatcher;
        entt::registry *scene;
    };
}

#endif //ENGINE25_APPLICATIONCONTEXT_H
