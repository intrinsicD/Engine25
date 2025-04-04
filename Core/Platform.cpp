//
// Created by alex on 04.04.25.
//

#include "Platform.h"
#include "Logger.h" // Assuming Logger is accessible
#include "Events.h" // Assuming Logger is accessible
#include "IRenderer.h" // Assuming Logger is accessible
#include "entt/signal/dispatcher.hpp" // Assuming Logger is accessible
#include "GLFW/glfw3.h"

namespace Bcg {
    // Static Error Callback Implementation
    static void errorCallback(int error, const char *description) {
        LOG_ERROR(fmt::format("GLFW Error ({}): {}", error, description));
    }

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto *context = static_cast<ApplicationContext *>(glfwGetWindowUserPointer(window));
        if (context) {
            context->windowDesc.width = width;
            context->windowDesc.height = height;

            // Trigger the resize event
            context->dispatcher->trigger(Events::Platform::WindowResizeEvent{width, height});

            if (context->platform->m_resizeCallback) {
                context->platform->m_resizeCallback(width, height);
            }

            if (context->renderer) {
                LOG_TODO("Recreate Swapchain in the renderer if needed!");
                //context->renderer->recreateSwapChain(width, height);
            }
        }
    }

    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto *context = static_cast<ApplicationContext *>(glfwGetWindowUserPointer(window));
        if (context) {
            context->keyboard.keys[key] = (action != GLFW_RELEASE);

            context->dispatcher->trigger(Events::Platform::KeyEvent{key, scancode, action, mods});

            if (context->platform->m_keyCallback) {
                context->platform->m_keyCallback(key, scancode, action, mods);
            }
        }
    }

    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
        auto *context = static_cast<ApplicationContext *>(glfwGetWindowUserPointer(window));
        if (context) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                context->mouse.leftButtonPressed = (action != GLFW_RELEASE);
            } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                context->mouse.middleButtonPressed = (action != GLFW_RELEASE);
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                context->mouse.rightButtonPressed = (action != GLFW_RELEASE);
            }

            context->dispatcher->trigger(Events::Platform::MouseButtonEvent{button, action, mods});

            if (context->platform->m_mouseButtonCallback) {
                context->platform->m_mouseButtonCallback(button, action, mods);
            }
        }
    }

    static void mouseMoveCallback(GLFWwindow *window, double xpos, double ypos) {
        auto *context = static_cast<ApplicationContext *>(glfwGetWindowUserPointer(window));
        if (context) {
            context->mouse.x = static_cast<int>(xpos);
            context->mouse.y = static_cast<int>(ypos);
            context->mouse.moves = true;

            context->dispatcher->trigger(Events::Platform::MouseMoveEvent{xpos, ypos});

            if (context->platform->m_mouseCallback) {
                context->platform->m_mouseCallback(xpos, ypos);
            }
        }
    }

    static void mouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
        auto *context = static_cast<ApplicationContext *>(glfwGetWindowUserPointer(window));
        if (context) {
            context->mouse.scroll_x = static_cast<int>(xoffset);
            context->mouse.scroll_y = static_cast<int>(yoffset);

            context->dispatcher->trigger(Events::Platform::MouseScrollEvent{xoffset, yoffset});
            if (context->platform->m_scrollCallback) {
                context->platform->m_scrollCallback(xoffset, yoffset);
            }
        }
    }

    static void charCallback(GLFWwindow *window, unsigned int codepoint) {
        auto *context = static_cast<ApplicationContext *>(glfwGetWindowUserPointer(window));
        if (context) {
            context->dispatcher->trigger(Events::Platform::CharEvent{codepoint});

            if (context->platform->m_charCallback) {
                context->platform->m_charCallback(codepoint);
            }
        }
    }

    static void dropCallback(GLFWwindow *window, int count, const char **paths) {
        auto *context = static_cast<ApplicationContext *>(glfwGetWindowUserPointer(window));
        if (context) {
            context->dispatcher->trigger(Events::Platform::DropEvent{count, paths});

            if (context->platform->m_dropCallback) {
                context->platform->m_dropCallback(count, paths);
            }
        }
    }

    static void windowCloseCallback(GLFWwindow *window) {
        auto *context = static_cast<ApplicationContext *>(glfwGetWindowUserPointer(window));
        if (context) {
            context->dispatcher->trigger(Events::Platform::WindowCloseEvent{});

            if (context->platform->m_windowCloseCallback) {
                context->platform->m_windowCloseCallback();
            }
        }
    }

    Platform::Platform() : IModule("GlfwPlatform", "0.1.0") {
        // Constructor can be empty or do minimal setup if needed
    }

    bool Platform::initialize(ApplicationContext *context) {
        if (m_app_context == nullptr && Platform::initialize(context)) {
            if (!context->renderer) {
                LOG_WARN("Platform: No renderer provided, skipping window creation.");
            } else {
                if (context->windowPtr) {
                    LOG_WARN("Platform: Window already created, skipping window creation.");
                } else {
                    glfwSetErrorCallback(errorCallback);
                    if (!glfwInit()) {
                        LOG_FATAL("Platform: Failed to initialize GLFW");
                        return false;
                    }

                    if (context->backend.type == "OpenGL") {
                        // Use values from the description struct
                        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context->backend.opengl.majorVersion);
                        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context->backend.opengl.minorVersion);

                        if(context->backend.opengl.profile == 0) {
                            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                        } else {
                            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
                        }
#ifdef __APPLE__
                        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
                    } else if (context->backend.type == "Vulkan") {
                        if (!glfwVulkanSupported()) {
                            LOG_FATAL("Platform: Vulkan requested but not supported by GLFW/System.");
                            glfwTerminate();
                            return false;
                        }
                        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                    } else {
                        LOG_FATAL(
                                fmt::format("Platform: Unsupported renderer type ({}) configured.", context->backend.type));
                    }

                    context->windowPtr = glfwCreateWindow(context->windowDesc.width, context->windowDesc.height, context->windowDesc.title.c_str(),
                                                       nullptr, nullptr);
                    if (!context->windowPtr) {
                        glfwTerminate();
                        LOG_FATAL("Platform: Failed to create GLFW window");
                    }

                    glfwMakeContextCurrent(context->windowPtr);

                    if (context->backend.vsync) {
                        LOG_INFO("Platform: VSync enabled");
                    } else {
                        LOG_INFO("Platform: VSync disabled");
                    }
                    glfwSwapInterval(context->backend.vsync); //Enable (1) /Disable (0) vsync

                    glfwSetWindowSizeCallback(context->windowPtr, framebufferResizeCallback);
                    glfwSetKeyCallback(context->windowPtr, keyCallback);
                    glfwSetMouseButtonCallback(context->windowPtr, mouseButtonCallback);
                    glfwSetCursorPosCallback(context->windowPtr, mouseMoveCallback);
                    glfwSetScrollCallback(context->windowPtr, mouseScrollCallback);
                    glfwSetCharCallback(context->windowPtr, charCallback);
                    glfwSetDropCallback(context->windowPtr, dropCallback);
                    glfwSetWindowCloseCallback(context->windowPtr, windowCloseCallback);

                    glfwSetWindowUserPointer(context->windowPtr, context);
                }
            }
            LOG_INFO("Platform: Context set and Window created successfully.");
        } else {
            LOG_WARN("Platform: Context already initialized or invalid context.");
            return false;
        }
        return true;
    }

    void Platform::shutdown() {
        if (m_app_context->windowPtr) {
            LOG_INFO("Platform: Destroying window...");
            glfwDestroyWindow(m_app_context->windowPtr);
            m_app_context->windowPtr = nullptr;
        }
        LOG_INFO("Platform: Terminating GLFW...");
        glfwTerminate();
        LOG_INFO("Platform: Shutdown complete.");
    }

    void Platform::pollEvents() {
        m_app_context->mouse.moves = false;

        glfwPollEvents();
    }

    bool Platform::shouldClose() const {
        return (m_app_context->windowPtr != nullptr) && glfwWindowShouldClose(m_app_context->windowPtr);
    }

    double Platform::getTime() const {
        return glfwGetTime();
    }
} // namespace Bcg

