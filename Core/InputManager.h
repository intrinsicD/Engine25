//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_INPUTMANAGER_H
#define ENGINE25_INPUTMANAGER_H

#include <array>

struct GLFWwindow;

namespace Bcg {
    constexpr int MAX_GLFW_KEYS = 350;          // A bit more than GLFW_KEY_LAST (~348)
    constexpr int MAX_GLFW_MOUSE_BUTTONS = 8;   // GLFW_MOUSE_BUTTON_LAST is 7

    class InputManager {
    public:
        InputManager();

        ~InputManager() = default;

        void initialize(GLFWwindow *window);

        void update();

        // --- Keyboard Query ---
        bool isKeyDown(int key) const;

        bool isKeyPressed(int key) const;

        bool isKeyReleased(int key) const;


        // --- Mouse Button Query ---
        bool isMouseButtonDown(int button) const;

        bool isMouseButtonPressed(int button) const;

        bool isMouseButtonReleased(int button) const;


        // --- Mouse Position Query ---
        std::pair<double, double> getMousePosition() const;

        std::pair<double, double> getMouseDelta() const;

        // --- Mouse Scroll Query ---
        std::pair<double, double> getScrollOffset() const;


    private:
        // --- GLFW Callback Handlers (Called by static callbacks) ---
        void handleKeyEvent(int key, int scancode, int action, int mods);

        void handleMouseButtonEvent(int button, int action, int mods);

        void handleCursorPosEvent(double xpos, double ypos);

        void handleScrollEvent(double xoffset, double yoffset);

        // --- Static GLFW Callbacks ---
        // These static functions forward events to the InputManager instance via window user pointer
        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

        static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

        static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

        static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

        // --- Internal State ---
        GLFWwindow *m_window = nullptr; // Non-owning pointer

        // Keyboard state
        std::array<bool, MAX_GLFW_KEYS> m_currentKeys;
        std::array<bool, MAX_GLFW_KEYS> m_previousKeys;

        // Mouse button state
        std::array<bool, MAX_GLFW_MOUSE_BUTTONS> m_currentMouseButtons;
        std::array<bool, MAX_GLFW_MOUSE_BUTTONS> m_previousMouseButtons;

        // Mouse position state
        double m_currentMouseX = 0.0;
        double m_currentMouseY = 0.0;
        double m_lastMouseX = 0.0;
        double m_lastMouseY = 0.0;
        bool m_firstMouseUpdate = true; // Handle initial delta correctly

        // Scroll state (per-frame offset)
        double m_scrollOffsetX = 0.0;
        double m_scrollOffsetY = 0.0;
    };
}

#endif //ENGINE25_INPUTMANAGER_H
