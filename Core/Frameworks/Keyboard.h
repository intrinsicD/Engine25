//
// Created by alex on 06.12.24.
//

#ifndef ENGINE25_KEYBOARD_H
#define ENGINE25_KEYBOARD_H

#include <set>

namespace Bcg {
    enum class Key {
        Unknown,
        // key A to Z
        A = 65, B, C, D, E, F, G,
        H, I, J, K, L, M, N, O,
        P, Q, R, S, T, U, V, W,
        X, Y, Z,

        // key 0 to 9
        Num0 = 48, Num1, Num2, Num3, Num4,
        Num5, Num6, Num7, Num8, Num9,

        // key F1 to F12
        F1 = 290, F2, F3, F4, F5,
        F6, F7, F8, F9, F10, F11, F12,

        // keypad keys
        KP0 = 320, KP1, KP2, KP3, KP4,
        KP5, KP6, KP7, KP8, KP9,
        KPDivide, KPMultiply, KPSubtract, KPAdd, KPEnter, KPDecimal,

        // arrow keys
        Left = 263, Right, Up, Down,

        // special keys
        Space = 32, Enter = 257, Escape = 256,
        LShift = 340, RShift, LCtrl = 341, RCtrl, LAlt = 342, RAlt,
        Tab = 258, Backspace = 259, Insert = 260, Delete, Home, End,
        PageUp = 266, PageDown, Pause = 284
    };

    class Keyboard {
    public:


        std::set<Key> pressed_keys;

        void PressKey(Key key) {
            pressed_keys.insert(key);
        }

        void ReleaseKey(Key key) {
            pressed_keys.erase(key);
        }

        bool IsPressed(Key key) {
            return pressed_keys.find(key) != pressed_keys.end();
        }
    };

    Key MapGlfwKey(int key);
}

#endif //ENGINE25_KEYBOARD_H
