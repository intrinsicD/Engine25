//
// Created by alex on 05.12.24.
//

#include "GLFWModule.h"
#include "GuiModule.h"
#include "Engine.h"
#include "GLFW/glfw3.h"
#include "Logger.h"
#include "RenderingModule.h"
#include "Pool.h"
#include "PoolHandle.h"
#include "MainLoop.h"
#include "InputModule.h"
#include "WindowComponent.h"
#include "ConfigFile.h"

namespace Bcg::Graphics {
    GLFWModule::GLFWModule() : Module("GLFWModule", "0.1") {
    }

    void GLFWModule::connect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().connect<&GLFWModule::on_initialize>(*this);
        Engine::get_dispatcher().sink<Events::Startup>().connect<&GLFWModule::on_startup>(*this);
        Engine::get_dispatcher().sink<Events::Synchronize>().connect<&GLFWModule::on_synchronize>(*this);
        Engine::get_dispatcher().sink<Events::Shutdown>().connect<&GLFWModule::on_shutdown>(*this);
        Module::connect_events();
    }

    void GLFWModule::disconnect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().disconnect<&GLFWModule::on_initialize>(*this);
        Engine::get_dispatcher().sink<Events::Startup>().disconnect<&GLFWModule::on_startup>(*this);
        Engine::get_dispatcher().sink<Events::Synchronize>().disconnect<&GLFWModule::on_synchronize>(*this);
        Engine::get_dispatcher().sink<Events::Shutdown>().disconnect<&GLFWModule::on_shutdown>(*this);
        Module::disconnect_events();
    }

    static void glfw_error_callback(int error, const char *description) {
        LOG_ERROR(fmt::format("GLFW Error: {} {}", error, description));
    }

    static void close_callback(GLFWwindow *window) {
        LOG_INFO("GLFWModule::close_callback: Close window");
        glfwSetWindowShouldClose(window, true);
        Engine::get_context().get<MainLoop>().Stop();
    }

    static bool mouse_logging = false;
    static bool key_logging = false;

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
        auto event = Events::Key{window, key, scancode, action, mode};
        InputModule::on_key(event);

        if (!GuiModule::WantCaptureKeyboard()) {
            Engine::get_dispatcher().trigger(event);

            auto &k_callbacks = Engine::get_context().get<KeyboardCallbacks>();
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
        InputModule::on_mouse_cursor(event);

        if (!GuiModule::WantCaptureMouse()) {
            Engine::get_dispatcher().trigger(event);
        }
    }

    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        auto event = Events::MouseButton{window, button, action, mods};
        InputModule::on_mouse_button(event);

        if (!GuiModule::WantCaptureMouse()) {
            Engine::get_dispatcher().trigger(event);
        }
    }

    static void mouse_scrolling(GLFWwindow *window, double xoffset, double yoffset) {
        auto event = Events::MouseScroll{window, xoffset, yoffset};
        InputModule::on_mouse_scroll(event);

        if (!GuiModule::WantCaptureMouse()) {
            Engine::get_dispatcher().trigger(event);
        }
    }

    static void window_resize_callback(GLFWwindow *window, int width, int height) {
        Engine::get_dispatcher().trigger(Events::WindowResize{window, width, height});
    }

    static void framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
        SetViewport(0, 0, width, height).Execute();
        Engine::get_dispatcher().trigger(Events::FramebufferResize{window, width, height});
    }

    static void drop_callback(GLFWwindow *window, int count, const char **paths) {
        for (int i = 0; i < count; ++i) {
            LOG_INFO(fmt::format("Dropped: {}", paths[i]));
        }

        Engine::get_dispatcher().trigger<Events::Drop>({window, count, paths});
    }

    GLFWwindow *GLFWModule::get_current_window() {
        return glfwGetCurrentContext();
    }

    Vector<Real, 2> GLFWModule::GetCursorPosition(GLFWwindow *window) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return {xpos, ypos};
    }

    void GLFWModule::on_initialize(const Events::Initialize &event) {
        Module::on_initialize(event);
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            LOG_FATAL("GLFWModule::on_initialize: Failed to initialize GLFW");
        }
        auto &renderer = Engine::get_context().get<RenderingModule>();
        auto &backend = renderer.GetBackend();
        if (!backend) {
            LOG_FATAL("GLFWModule::on_initialize: No backend set, please set a backend before initializing GLFW");
        }
        LOG_INFO(fmt::format("GLFWModule::on_initialize: Starting GLFW context, {} {}", backend.name, backend.version));

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
                LOG_FATAL("GLFWModule::on_initialize: Unknown backend type");
        }

        Engine::get_context().emplace<Pool<Window> >("window_pool");
    }

    void GLFWModule::on_startup(const Events::Startup &event) {
        Module::on_startup(event);

        if (!Engine::get_context().find<WindowComponent>()) {
            auto &window_pool = Engine::get_context().get<Pool<Window> >();
            auto window_handle = window_pool.CreateHandle();
            Window window;
            if (!Engine::get_context().find<Window>()) {
                window.width = Config::get_int("window.width");
                window.height = Config::get_int("window.height");
                window.title = Config::get_string("window.title");
                LOG_INFO("GLFWModule::on_startup: Create window from default");
            } else {
                window = Engine::get_context().get<Window>();
                Engine::get_context().erase<Window>();
                LOG_INFO("GLFWModule::on_startup: Create window from Engine::get_context()");
            }

            window_handle->width = window.width;
            window_handle->height = window.height;
            window_handle->title = window.title;

            window_handle->handle = glfwCreateWindow(window_handle->width, window_handle->height, window_handle->title.c_str(),
                                                     nullptr, nullptr);
            if (!window_handle->handle) {
                LOG_FATAL("GLFWModule::on_startup: Failed to create window");
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
            Engine::get_context().emplace<WindowComponent>(window_handle);
        }

        auto &k_callbacks = Engine::get_context().get<KeyboardCallbacks>();

        k_callbacks.SetPressKeymapCallback(Key::Escape, []() {
            if (glfwGetKey(Engine::get_context().get<WindowComponent>()->handle, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                close_callback(Engine::get_context().get<WindowComponent>()->handle);
            }
        });
        k_callbacks.SetPressKeymapCallback(Key::C, []() {
            if (glfwGetKey(Engine::get_context().get<WindowComponent>()->handle, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                LogThisFrame().Execute();
            }
        });
        k_callbacks.SetPressKeymapCallback(Key::M, []() {
            if (glfwGetKey(Engine::get_context().get<WindowComponent>()->handle, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                if (!mouse_logging) {
                    mouse_logging = true;
                    InputModule::enable_mouse_logging();
                } else {
                    mouse_logging = false;
                    InputModule::disable_mouse_logging();
                }
            }
        });
        k_callbacks.SetPressKeymapCallback(Key::K, []() {
            if (glfwGetKey(Engine::get_context().get<WindowComponent>()->handle, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                if (!key_logging) {
                    key_logging = true;
                    InputModule::enable_key_logging();
                } else {
                    key_logging = false;
                    InputModule::disable_key_logging();
                }
            }
        });
    }

    void GLFWModule::on_synchronize(const Events::Synchronize &event) {
        Module::on_synchronize(event);
        auto &loop = Engine::get_context().get<MainLoop>();
        loop.end.Next().AddCommand(std::make_shared<PollEvents>());
        loop.end.Next().AddCommand(std::make_shared<SwapBuffers>());
    }

    void GLFWModule::on_shutdown(const Events::Shutdown &event) {
        Module::on_shutdown(event);
        glfwTerminate();
    }

    float GLFWModule::GetDpiScaling() {
        auto &window_handle = Engine::get_context().get<WindowComponent>();
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
        glfwSwapBuffers(Engine::get_context().get<WindowComponent>()->handle);
        LOG_FRAME("Command::SwapBuffers");
    }
}
