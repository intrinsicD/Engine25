//
// Created by alex on 04.04.25.
//

#include "Platform.h"
#include "Logger.h" // Assuming Logger is accessible
#include "Events.h" // Assuming Logger is accessible
#include "VulkanRenderer.h" // Assuming Logger is accessible
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

    Platform::Platform() : IModule("GlfwPlatform", "0.1.1") {} // Increment version

    bool Platform::initialize(ApplicationContext *context) {
        // Use base class initialize first
        if (!IModule::initialize(context)) {
            LOG_ERROR("Platform: Base module initialization failed.");
            return false;
        }

        // Ensure context is valid after base class call
        if (!m_app_context) {
            LOG_FATAL("Platform: ApplicationContext is null after base initialization.");
            m_initialized = false; // Reset flag
            return false;
        }

        // Store pointer back to platform in context for callbacks
        m_app_context->platform = this;

        if (m_app_context->windowPtr) {
            LOG_WARN("Platform: Window handle already exists in context. Assuming external creation or re-initialization.");
            // Re-attach callbacks if needed, but don't create window again.
            glfwSetWindowUserPointer(m_app_context->windowPtr, m_app_context); // Ensure user pointer is set
            // Consider re-setting callbacks here if re-initialization is a use case
        } else {
            LOG_INFO("Platform: Initializing GLFW...");
            glfwSetErrorCallback(errorCallback);
            if (!glfwInit()) {
                LOG_FATAL("Platform: Failed to initialize GLFW");
                m_initialized = false; // Reset flag
                return false;
            }

            // --- Set Window Hints based on Backend ---
            if (m_app_context->backend.type == "OpenGL") {
                LOG_INFO("Platform: Configuring for OpenGL backend.");
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_app_context->backend.opengl.majorVersion);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_app_context->backend.opengl.minorVersion);
                glfwWindowHint(GLFW_OPENGL_PROFILE, m_app_context->backend.opengl.profile == 0 ? GLFW_OPENGL_CORE_PROFILE : GLFW_OPENGL_COMPAT_PROFILE);
#ifdef __APPLE__
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API); // Explicitly set GL API

            } else if (m_app_context->backend.type == "Vulkan") {
                LOG_INFO("Platform: Configuring for Vulkan backend.");
                if (!glfwVulkanSupported()) {
                    LOG_FATAL("Platform: Vulkan requested but not supported by GLFW/System.");
                    glfwTerminate();
                    m_initialized = false; // Reset flag
                    return false;
                }
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // <<< Crucial for Vulkan

            } else {
                LOG_FATAL(fmt::format("Platform: Unsupported backend type ({}) configured.", m_app_context->backend.type));
                glfwTerminate();
                m_initialized = false; // Reset flag
                return false;
            }

            // --- Create Window ---
            LOG_INFO(fmt::format("Platform: Creating window {}x{} '{}'",
                                 m_app_context->windowDesc.width, m_app_context->windowDesc.height, m_app_context->windowDesc.title));
            m_app_context->windowPtr = glfwCreateWindow(
                    m_app_context->windowDesc.width,
                    m_app_context->windowDesc.height,
                    m_app_context->windowDesc.title.c_str(),
                    nullptr, // Monitor (for fullscreen)
                    nullptr  // Share context (for multiple GL windows)
            );

            if (!m_app_context->windowPtr) {
                glfwTerminate();
                LOG_FATAL("Platform: Failed to create GLFW window");
                m_initialized = false; // Reset flag
                return false;
            }
            LOG_INFO("Platform: Window created successfully.");

            // --- OpenGL Specific Post-Creation Steps ---
            if (m_app_context->backend.type == "OpenGL") {
                glfwMakeContextCurrent(m_app_context->windowPtr); // Make the OpenGL context current *ONLY for OpenGL*
                // Check for errors after making context current
                const char* description;
                int err_code = glfwGetError(&description);
                if (err_code != GLFW_NO_ERROR) {
                    LOG_ERROR(fmt::format("GLFW Error after MakeContextCurrent ({}): {}", err_code, description ? description : "Unknown"));
                    // Decide if this is fatal
                } else {
                    LOG_INFO("Platform: OpenGL context made current.");
                }


                // Set VSync *ONLY for OpenGL*
                int vsync_val = m_app_context->backend.vsync ? 1 : 0;
                glfwSwapInterval(vsync_val);
                err_code = glfwGetError(&description);
                if (err_code != GLFW_NO_ERROR) {
                    LOG_ERROR(fmt::format("GLFW Error setting swap interval ({}): {}", err_code, description ? description : "Unknown"));
                } else {
                    LOG_INFO(fmt::format("Platform: VSync set to {} for OpenGL.", vsync_val));
                }

            } else if (m_app_context->backend.type == "Vulkan") {
                LOG_INFO(fmt::format("Platform: VSync for Vulkan controlled by presentation mode (Requested VSync: {}).", m_app_context->backend.vsync));
                // VSync is handled by VkPresentModeKHR in VulkanRenderer swapchain creation.
                // FIFO mode typically corresponds to VSync ON.
            }

            // --- Set Callbacks ---
            glfwSetWindowUserPointer(m_app_context->windowPtr, m_app_context); // Set user pointer BEFORE callbacks
            glfwSetFramebufferSizeCallback(m_app_context->windowPtr, framebufferResizeCallback);
            glfwSetKeyCallback(m_app_context->windowPtr, keyCallback);
            glfwSetMouseButtonCallback(m_app_context->windowPtr, mouseButtonCallback);
            glfwSetCursorPosCallback(m_app_context->windowPtr, mouseMoveCallback);
            glfwSetScrollCallback(m_app_context->windowPtr, mouseScrollCallback);
            glfwSetCharCallback(m_app_context->windowPtr, charCallback);
            glfwSetDropCallback(m_app_context->windowPtr, dropCallback);
            glfwSetWindowCloseCallback(m_app_context->windowPtr, windowCloseCallback);
            LOG_INFO("Platform: GLFW Callbacks set.");
        }

        // m_initialized is set by IModule::initialize
        LOG_INFO("Platform Initialization Complete.");
        return true;
    }

    void Platform::shutdown() {
        LOG_INFO("Platform Shutting Down...");
        if (m_app_context && m_app_context->windowPtr) {
            LOG_INFO("Platform: Destroying window...");
            glfwDestroyWindow(m_app_context->windowPtr);
            m_app_context->windowPtr = nullptr; // Clear the pointer in the context
        } else {
            LOG_INFO("Platform: No window to destroy.");
        }
        LOG_INFO("Platform: Terminating GLFW...");
        glfwTerminate();

        // Call base shutdown last
        IModule::shutdown();
        LOG_INFO("Platform Shutdown Complete.");
    }

    void Platform::pollEvents() {
        if (!isInitialized()) return; // Don't poll if not initialized

        // Reset per-frame input state *before* polling
        if(m_app_context) {
            m_app_context->mouse.moves = false;
            m_app_context->mouse.scroll_x = 0.0;
            m_app_context->mouse.scroll_y = 0.0;
            // Keyboard state is updated directly in callback
        }

        glfwPollEvents(); // Processes events and triggers callbacks
    }

    bool Platform::shouldClose() const {
        // Check if initialized and window exists before polling GLFW
        return isInitialized() && m_app_context && m_app_context->windowPtr && glfwWindowShouldClose(m_app_context->windowPtr);
    }

    double Platform::getTime() const {
        return glfwGetTime();
    }

    void Platform::swapBuffers() {
        if (isInitialized() && m_app_context && m_app_context->windowPtr) {
            // *** Only swap buffers for OpenGL ***
            if (m_app_context->backend.type == "OpenGL") {
                glfwSwapBuffers(m_app_context->windowPtr);
            }
            // No-op for Vulkan - presentation handled by VulkanRenderer
        } else {
            // Log warning only if initialized but window is somehow null
            if(isInitialized()) {
                LOG_WARN("Platform::swapBuffers called but window pointer is null.");
            }
        }
    }

} // namespace Bcg

