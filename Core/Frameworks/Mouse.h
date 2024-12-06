//
// Created by alex on 06.12.24.
//

#ifndef ENGINE25_MOUSE_H
#define ENGINE25_MOUSE_H

#include "glm/glm.hpp"

namespace Bcg {
    class Mouse {
    public:
        Mouse() = default;

        ~Mouse() = default;

        enum class State{
            Idle,
            Move,
            Scroll,
            Drag
        }state = State::Idle;

        struct Button {
            enum class State{
                Idle,
                Press,
                Release,
                Repeat
            }state;

            glm::vec2 last_press;
            glm::vec2 last_release;
        };

        struct Wheel {
            enum class State{
                Idle,
                Scroll,
                Press,
                Release,
                Repeat
            }state;
            glm::vec2 scroll_offset;
            glm::vec2 last_press;
            glm::vec2 last_release;
        };

        Button left;
        Wheel wheel;
        Button right;

        bool any_pressed() const {
            return left.state == Button::State::Press || right.state == Button::State::Press || wheel.state == Wheel::State::Press;
        }

        glm::vec2 cursor_position;
    };
}

#endif //ENGINE25_MOUSE_H
