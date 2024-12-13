//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_ENGINEEVENTS_H
#define ENGINE25_ENGINEEVENTS_H

struct GLFWwindow;

namespace Bcg::Events {
    struct Initialize {

    };

    struct Startup {

    };

    struct BeginFrameEvent {

    };

    struct UpdateSystems{

    };

    struct EndFrameEvent {

    };

    struct Synchronize {

    };

    struct Shutdown {

    };

    namespace Gui {
        struct RenderMenu {

        };

        struct RenderGui {

        };
    }



    struct Key {
        GLFWwindow *window;
        int key, scancode, action, mode;
    };

    struct MouseCursor {
        GLFWwindow *window;
        double xpos, ypos;
    };

    struct MouseButton {
        GLFWwindow *window;
        int button, action, mods;
    };

    struct MouseScroll {
        GLFWwindow *window;
        double xoffset, yoffset;
    };

    struct WindowResize {
        GLFWwindow *window;
        int width, height;
    };

    struct FramebufferResize {
        GLFWwindow *window;
        int width, height;
    };

    struct WindowClose {
        GLFWwindow *window;
    };

    struct Drop {
        GLFWwindow *window;
        int count;
        const char **paths;
    };
}

#endif //ENGINE25_ENGINEEVENTS_H
