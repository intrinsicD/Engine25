//
// Created by alex on 05.12.24.
//

#include "GLFWModule.h"
#include "GuiModule.h"
#include "Engine.h"
#include "GLFW/glfw3.h"
#include "LoggingMacros.h"
#include "RenderingModule.h"
#include "Pool.h"
#include "PoolHandle.h"
#include "MainLoop.h"
#include "InputModule.h"
#include "WindowComponent.h"
#include "imgui.h"

namespace Bcg {
    GLFWModule::GLFWModule() : Module("GLFWModule", "0.1") {
    }

    void GLFWModule::ConnectEvents() {
        Engine::GetDispatcher().sink<Events::Initialize>().connect<&GLFWModule::OnInitialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().connect<&GLFWModule::OnStartup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().connect<&GLFWModule::OnSynchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().connect<&GLFWModule::OnShutdown>(*this);
        Module::ConnectEvents();
    }

    void GLFWModule::DisconnectEvents() {
        Engine::GetDispatcher().sink<Events::Initialize>().disconnect<&GLFWModule::OnInitialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().disconnect<&GLFWModule::OnStartup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().disconnect<&GLFWModule::OnSynchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().disconnect<&GLFWModule::OnShutdown>(*this);
        Module::DisconnectEvents();
    }

    static void glfw_error_callback(int error, const char *description) {
        std::string message = "GLFW Error " + std::to_string(error) + ", " + description + "\n";
        LOG_ERROR(fmt::format("GLFW: {}", message));
    }

    static void close_callback(GLFWwindow *window) {
        LOG_INFO("GLFWModule::close_callback: Close window");
        glfwSetWindowShouldClose(window, true);
        Engine::GetContext().get<MainLoop>().Stop();
    }

    static bool mouse_logging = false;
    static bool key_logging = false;

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
        auto event = Events::Key{window, key, scancode, action, mode};
        InputModule::OnKey(event);

        if (!GuiModule::WantCaptureKeyboard()) {
            Engine::GetDispatcher().trigger(event);

            auto &k_callbacks = Engine::GetContext().get<KeyboardCallbacks>();
            auto bcg_key = MapGlfwKey(key);
            if (action == GLFW_PRESS) {
                k_callbacks.TriggerPressKeyCallback(bcg_key);
            } else if (action == GLFW_RELEASE) {
                k_callbacks.TriggerReleaseKeyCallback(bcg_key);
            }
        }
    }

    static void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos) {
        auto event = Events::MouseCursor{window, xpos, ypos};
        InputModule::OnMouseCursor(event);

        if (!GuiModule::WantCaptureMouse()) {
            Engine::GetDispatcher().trigger(event);
        }
    }

    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        auto event = Events::MouseButton{window, button, action, mods};
        InputModule::OnMouseButton(event);

        if (!GuiModule::WantCaptureMouse()) {
            Engine::GetDispatcher().trigger(event);
        }
    }

    static void mouse_scrolling(GLFWwindow *window, double xoffset, double yoffset) {
        auto event = Events::MouseScroll{window, xoffset, yoffset};
        InputModule::OnMouseScroll(event);

        if (!GuiModule::WantCaptureMouse()) {
            Engine::GetDispatcher().trigger(event);
        }
    }

    static void window_resize_callback(GLFWwindow *window, int width, int height) {
        Engine::GetDispatcher().trigger(Events::WindowResize{window, width, height});
    }

    static void framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
        SetViewport(0, 0, width, height).Execute();
        Engine::GetDispatcher().trigger(Events::FramebufferResize{window, width, height});
    }

    static void drop_callback(GLFWwindow *window, int count, const char **paths) {
        for (int i = 0; i < count; ++i) {
            LOG_INFO(fmt::format("Dropped: {}", paths[i]));
        }

        Engine::GetDispatcher().trigger<Events::Drop>({window, count, paths});
    }

    GLFWwindow *GLFWModule::GetCurrentWindow() {
        return glfwGetCurrentContext();
    }

    glm::vec2 GLFWModule::GetCursorPosition(GLFWwindow *window) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return {xpos, ypos};
    }

    void GLFWModule::OnInitialize(const Events::Initialize &event) {
        Module::OnInitialize(event);
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            LOG_FATAL("GLFWModule::OnInitialize: Failed to initialize GLFW");
        }
        auto &renderer = Engine::GetContext().get<RenderingModule>();
        auto &backend = renderer.GetBackend();
        if (!backend) {
            LOG_FATAL("GLFWModule::OnInitialize: No backend set, please set a backend before initializing GLFW");
        }
        LOG_INFO(fmt::format("GLFWModule::OnInitialize: Starting GLFW context, {} {}", backend.name, backend.version));

        GLFWwindow *glfw_window = nullptr;
        switch (backend.type) {
            case RenderingModule::Backend::Type::OpenGL:
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                break;
            case RenderingModule::Backend::Type::Vulkan:
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                break;
            default:
                LOG_FATAL("GLFWModule::OnInitialize: Unknown backend type");
        }

        Engine::GetContext().emplace<Pool<Window> >("window_pool");
    }

    void GLFWModule::OnStartup(const Events::Startup &event) {
        Module::OnStartup(event);

        if (!Engine::GetContext().find<WindowComponent>()) {
            auto &window_pool = Engine::GetContext().get<Pool<Window> >();
            auto window_handle = window_pool.Create();
            Window window;
            if (!Engine::GetContext().find<Window>()) {
                window.width = 1280;
                window.height = 720;
                window.title = "Engine 25";
                LOG_INFO("GLFWModule::OnStartup: Create window from default");
            } else {
                window = Engine::GetContext().get<Window>();
                Engine::GetContext().erase<Window>();
                LOG_INFO("GLFWModule::OnStartup: Create window from Engine::GetContext()");
            }

            window_handle->width = window.width;
            window_handle->height = window.height;
            window_handle->title = window.title;

            window_handle->handle = glfwCreateWindow(window_handle->width, window_handle->height, window_handle->title,
                                                     nullptr, nullptr);
            if (!window_handle->handle) {
                LOG_FATAL("GLFWModule::OnStartup: Failed to create window");
            }

            glfwMakeContextCurrent(window_handle->handle);
            glfwSetWindowCloseCallback(window_handle->handle, close_callback);

            glfwSetKeyCallback(window_handle->handle, key_callback);
            glfwSetCursorPosCallback(window_handle->handle, mouse_cursor_callback);
            glfwSetMouseButtonCallback(window_handle->handle, mouse_button_callback);
            glfwSetScrollCallback(window_handle->handle, mouse_scrolling);

            glfwSetWindowSizeCallback(window_handle->handle, window_resize_callback);
            glfwSetFramebufferSizeCallback(window_handle->handle, framebuffer_resize_callback);
            glfwSetDropCallback(window_handle->handle, drop_callback);
            Engine::GetContext().emplace<WindowComponent>(window_handle);
        }

        auto &k_callbacks = Engine::GetContext().get<KeyboardCallbacks>();

        k_callbacks.SetPressKeymapCallback(Key::Escape, []() {
            if (glfwGetKey(Engine::GetContext().get<WindowComponent>()->handle, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                close_callback(Engine::GetContext().get<WindowComponent>()->handle);
            }
        });
        k_callbacks.SetPressKeymapCallback(Key::C, []() {
            if (glfwGetKey(Engine::GetContext().get<WindowComponent>()->handle, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                LogThisFrame().Execute();
            }
        });
        k_callbacks.SetPressKeymapCallback(Key::M, []() {
            if (glfwGetKey(Engine::GetContext().get<WindowComponent>()->handle, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                if (!mouse_logging) {
                    mouse_logging = true;
                    InputModule::EnableMouseLogging();
                } else {
                    mouse_logging = false;
                    InputModule::DisableMouseLogging();
                }
            }
        });
        k_callbacks.SetPressKeymapCallback(Key::K, []() {
            if (glfwGetKey(Engine::GetContext().get<WindowComponent>()->handle, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                if (!key_logging) {
                    key_logging = true;
                    InputModule::EnableKeyLogging();
                } else {
                    key_logging = false;
                    InputModule::DisableKeyLogging();
                }
            }
        });
    }

    void GLFWModule::OnSynchronize(const Events::Synchronize &event) {
        Module::OnSynchronize(event);
        auto &loop = Engine::GetContext().get<MainLoop>();
        loop.end.Next().AddCommand(std::make_shared<PollEvents>());
        loop.end.Next().AddCommand(std::make_shared<SwapBuffers>());
    }

    void GLFWModule::OnShutdown(const Events::Shutdown &event) {
        Module::OnShutdown(event);
        glfwTerminate();
    }

    float GLFWModule::GetDpiScaling() {
        auto &window_handle = Engine::GetContext().get<WindowComponent>();
        float dpi_scaling_factor;
        glfwGetWindowContentScale(window_handle->handle, &dpi_scaling_factor, &dpi_scaling_factor);
        return dpi_scaling_factor;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Commands
    //------------------------------------------------------------------------------------------------------------------

    void PollEvents::Execute() const {
        glfwPollEvents();
        LOG_FRAME("Command::PollEvents");
    }

    void SwapBuffers::Execute() const {
        glfwSwapBuffers(Engine::GetContext().get<WindowComponent>()->handle);
        LOG_FRAME("Command::SwapBuffers");
    }
}
