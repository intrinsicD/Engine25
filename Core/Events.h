//
// Created by alex on 04.04.25.
//

#ifndef ENGINE25_EVENTS_H
#define ENGINE25_EVENTS_H

namespace Bcg::Events{
    namespace Platform{
        struct WindowResizeEvent{
            int width;
            int height;
        };

        struct KeyEvent{
            int key;
            int scancode;
            int action;
            int mods;
        };

        struct MouseButtonEvent{
            int button;
            int action;
            int mods;
        };

        struct MouseMoveEvent{
            double x;
            double y;
        };

        struct MouseScrollEvent{
            double xOffset;
            double yOffset;
        };

        struct CharEvent{
            unsigned int codepoint;
        };

        struct DropEvent{
            int count;
            const char **paths;
        };

        struct WindowCloseEvent{
            // Add any additional data if needed
        };
    }
}

#endif //ENGINE25_EVENTS_H
