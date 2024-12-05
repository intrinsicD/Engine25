//
// Created by alex on 05.12.24.
//

#include "GLFWModule.h"
#include "Engine.h"
#include "GLFW/glfw3.h"
#include "LoggingMacros.h"
#include "RenderingModule.h"
#include "Pool.h"
#include "PoolHandle.h"
#include "MainLoop.h"
#include "WindowComponent.h"

namespace Bcg {
    GLFWModule::GLFWModule() : Module("GLFWModule", "0.1") {}

    void GLFWModule::connect_events() {
        Engine::GetDispatcher().sink<Events::Initialize>().connect<&GLFWModule::on_initialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().connect<&GLFWModule::on_startup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().connect<&GLFWModule::on_synchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().connect<&GLFWModule::on_shutdown>(*this);
        Module::connect_events();
    }

    void GLFWModule::disconnect_events() {
        Engine::GetDispatcher().sink<Events::Initialize>().disconnect<&GLFWModule::on_initialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().disconnect<&GLFWModule::on_startup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().disconnect<&GLFWModule::on_synchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().disconnect<&GLFWModule::on_shutdown>(*this);
        Module::disconnect_events();
    }

    static void glfw_error_callback(int error, const char *description) {
        std::string message = "GLFW Error " + std::to_string(error) + ", " + description + "\n";
        LOG_ERROR(fmt::format("GLFW: {}", message));
    }

    static void close_callback(GLFWwindow *window) {
        glfwSetWindowShouldClose(window, true);
        Engine::GetContext().get<MainLoop>().Stop();
    }

    void GLFWModule::on_initialize(const Events::Initialize &event) {
        Module::on_initialize(event);
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            LOG_FATAL("Failed to initialize GLFW");
        }
        auto &renderer = Engine::GetContext().get<RenderingModule>();
        auto &backend = renderer.GetBackend();
        if (!backend) {
            LOG_FATAL("No backend set, please set a backend before initializing GLFW");
        }
        LOG_INFO(fmt::format("Starting GLFW context, {} {}", backend.name, backend.version));

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
                LOG_FATAL("Unknown backend type");
        }

        Engine::GetContext().emplace<Pool<Window>>("window_pool");
    }

    void GLFWModule::on_startup(const Events::Startup &event) {
        Module::on_startup(event);

        if (!Engine::GetContext().find<WindowComponent>()) {
            auto &window_pool = Engine::GetContext().get<Pool<Window>>();
            auto window_handle = window_pool.Create();
            Window window;
            if (!Engine::GetContext().find<Window>()) {
                window.width = 1280;
                window.height = 720;
                window.title = "Engine 25";
                LOG_INFO("Create window from default");
            } else {
                window = Engine::GetContext().get<Window>();
                Engine::GetContext().erase<Window>();
                LOG_INFO("Create window from Engine::GetContext()");
            }

            window_handle->width = window.width;
            window_handle->height = window.height;
            window_handle->title = window.title;

            window_handle->handle = glfwCreateWindow(window_handle->width, window_handle->height, window_handle->title,
                                                     nullptr, nullptr);
            if (!window_handle->handle) {
                LOG_FATAL("Failed to create GLFW window");
            }

            glfwMakeContextCurrent(window_handle->handle);
            glfwSetWindowCloseCallback(window_handle->handle, close_callback);
            Engine::GetContext().emplace<WindowComponent>(window_handle);
        }
    }

    void GLFWModule::on_synchronize(const Events::Synchronize &event) {
        auto &loop = Engine::GetContext().get<MainLoop>();
        loop.end.Next().AddCommand(std::make_shared<PollEvents>());
        loop.end.Next().AddCommand(std::make_shared<SwapBuffers>());
    }

    void GLFWModule::on_shutdown(const Events::Shutdown &event) {
        Module::on_shutdown(event);
        glfwTerminate();
    }

    //------------------------------------------------------------------------------------------------------------------
    // Commands
    //------------------------------------------------------------------------------------------------------------------

    void PollEvents::Execute() const {
        glfwPollEvents();
    }

    void SwapBuffers::Execute() const {
        glfwSwapBuffers(Engine::GetContext().get<WindowComponent>()->handle);
    }
}