//
// Created by alex on 02.04.25.
//
#include "InputManager.h" // Include the header
#include "GLFW/glfw3.h"        // Include GLFW
#include <stdexcept>          // For runtime_error
#include "Logger.h"

namespace Bcg {
    InputManager::InputManager() {
        // Initialize state arrays to false
        m_currentKeys.fill(false);
        m_previousKeys.fill(false);
        m_currentMouseButtons.fill(false);
        m_previousMouseButtons.fill(false);
    }

    void InputManager::initialize(GLFWwindow *window) {
        if (!window) {
            throw std::runtime_error("[InputManager] Provided GLFW window was null.");
        }
        m_window = window;

        // Store 'this' pointer in the window's user pointer so static callbacks can access the instance
        glfwSetWindowUserPointer(m_window, this);

        // Register static callbacks with GLFW
        glfwSetKeyCallback(m_window, InputManager::keyCallback);
        glfwSetMouseButtonCallback(m_window, InputManager::mouseButtonCallback);
        glfwSetCursorPosCallback(m_window, InputManager::cursorPosCallback);
        glfwSetScrollCallback(m_window, InputManager::scrollCallback);

        // Initialize mouse position
        glfwGetCursorPos(m_window, &m_currentMouseX, &m_currentMouseY);
        m_lastMouseX = m_currentMouseX;
        m_lastMouseY = m_currentMouseY;
        m_firstMouseUpdate = true;

        LOG_INFO(fmt::format("InputManager: Initialized and callbacks registered"));
    }

    void InputManager::update() {
        // This function transitions the current state to the previous state
        // It should be called *after* glfwPollEvents() and *before* processing input for the new frame.

        // Update previous key states
        m_previousKeys = m_currentKeys; // Efficient copy for std::array

        // Update previous mouse button states
        m_previousMouseButtons = m_currentMouseButtons;

        // Update previous mouse position
        // Only update m_lastMouseX/Y if it's not the very first frame after init
        // to avoid large initial delta if mouse starts away from (0,0)
        if (m_firstMouseUpdate) {
            // On the first update after init, set last position = current position
            glfwGetCursorPos(m_window, &m_currentMouseX, &m_currentMouseY); // Ensure current is up-to-date
            m_lastMouseX = m_currentMouseX;
            m_lastMouseY = m_currentMouseY;
            m_firstMouseUpdate = false;
        } else {
            m_lastMouseX = m_currentMouseX;
            m_lastMouseY = m_currentMouseY;
            // m_currentMouseX/Y will be updated by the callback during the *next* poll event phase
        }


        // Reset scroll offset for the new frame (it accumulates during polling)
        m_scrollOffsetX = 0.0;
        m_scrollOffsetY = 0.0;
    }

    // --- Keyboard Query Implementations ---

    bool InputManager::isKeyDown(int key) const {
        if (key >= 0 && key < MAX_GLFW_KEYS) {
            return m_currentKeys[key];
        }
        return false; // Invalid key
    }

    bool InputManager::isKeyPressed(int key) const {
        if (key >= 0 && key < MAX_GLFW_KEYS) {
            return m_currentKeys[key] && !m_previousKeys[key];
        }
        return false; // Invalid key
    }

    bool InputManager::isKeyReleased(int key) const {
        if (key >= 0 && key < MAX_GLFW_KEYS) {
            return !m_currentKeys[key] && m_previousKeys[key];
        }
        return false; // Invalid key
    }

    // --- Mouse Button Query Implementations ---

    bool InputManager::isMouseButtonDown(int button) const {
        if (button >= 0 && button < MAX_GLFW_MOUSE_BUTTONS) {
            return m_currentMouseButtons[button];
        }
        return false; // Invalid button
    }

    bool InputManager::isMouseButtonPressed(int button) const {
        if (button >= 0 && button < MAX_GLFW_MOUSE_BUTTONS) {
            return m_currentMouseButtons[button] && !m_previousMouseButtons[button];
        }
        return false; // Invalid button
    }

    bool InputManager::isMouseButtonReleased(int button) const {
        if (button >= 0 && button < MAX_GLFW_MOUSE_BUTTONS) {
            return !m_currentMouseButtons[button] && m_previousMouseButtons[button];
        }
        return false; // Invalid button
    }


    // --- Mouse Position Query Implementations ---

    std::pair<double, double> InputManager::getMousePosition() const {
        // Return the most recently captured position
        return {m_currentMouseX, m_currentMouseY};
        // Or, if you want the absolute latest:
        // double x, y;
        // glfwGetCursorPos(m_window, &x, &y);
        // return {x, y};
        // But usually relying on the callback-updated values is fine and avoids extra GLFW calls.
    }

    std::pair<double, double> InputManager::getMouseDelta() const {
        // Avoid large delta on the very first frame
        if (m_firstMouseUpdate) {
            return {0.0, 0.0};
        }
        return {m_currentMouseX - m_lastMouseX, m_currentMouseY - m_lastMouseY};
    }

    // --- Mouse Scroll Query Implementations ---

    std::pair<double, double> InputManager::getScrollOffset() const {
        return {m_scrollOffsetX, m_scrollOffsetY};
    }


    // --- Private Callback Handlers ---

    void InputManager::handleKeyEvent(int key, int scancode, int action, int mods) {
        if (key >= 0 && key < MAX_GLFW_KEYS) {
            if (action == GLFW_PRESS) {
                m_currentKeys[key] = true;
            } else if (action == GLFW_RELEASE) {
                m_currentKeys[key] = false;
            }
            // GLFW_REPEAT events are ignored for state tracking, but the key remains true
        }
        // TODO: Optionally dispatch KeyPressedEvent/KeyReleasedEvent here
    }

    void InputManager::handleMouseButtonEvent(int button, int action, int mods) {
        if (button >= 0 && button < MAX_GLFW_MOUSE_BUTTONS) {
            if (action == GLFW_PRESS) {
                m_currentMouseButtons[button] = true;
            } else if (action == GLFW_RELEASE) {
                m_currentMouseButtons[button] = false;
            }
        }
        // TODO: Optionally dispatch MouseButtonPressedEvent/MouseButtonReleasedEvent here
    }

    void InputManager::handleCursorPosEvent(double xpos, double ypos) {
        // Update current position immediately when the callback occurs
        m_currentMouseX = xpos;
        m_currentMouseY = ypos;
        // TODO: Optionally dispatch MouseMovedEvent here (pass current and delta)
    }

    void InputManager::handleScrollEvent(double xoffset, double yoffset) {
        // Accumulate scroll offsets received since the last frame update
        m_scrollOffsetX += xoffset;
        m_scrollOffsetY += yoffset;
        // TODO: Optionally dispatch MouseScrolledEvent here
    }


// --- Static GLFW Callbacks Implementation ---

    void InputManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        // Retrieve the InputManager instance stored in the window's user pointer
        InputManager *inputManager = static_cast<InputManager *>(glfwGetWindowUserPointer(window));
        if (inputManager) {
            inputManager->handleKeyEvent(key, scancode, action, mods);
        }
    }

    void InputManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
        InputManager *inputManager = static_cast<InputManager *>(glfwGetWindowUserPointer(window));
        if (inputManager) {
            inputManager->handleMouseButtonEvent(button, action, mods);
        }
    }

    void InputManager::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
        InputManager *inputManager = static_cast<InputManager *>(glfwGetWindowUserPointer(window));
        if (inputManager) {
            inputManager->handleCursorPosEvent(xpos, ypos);
        }
    }

    void InputManager::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
        InputManager *inputManager = static_cast<InputManager *>(glfwGetWindowUserPointer(window));
        if (inputManager) {
            inputManager->handleScrollEvent(xoffset, yoffset);
        }
    }
}