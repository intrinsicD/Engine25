//
// Created by alex on 02.04.25.
//

#include "Application.h"
#include "Logger.h"
#include "ConfigFile.h"
#include "GLFW/glfw3.h"
#include "RendererFactory.h"
#include "BackendDescriptor.h"

namespace Bcg {

    void glfw_error_callback(int error, const char *description) {
        LOG_ERROR(fmt::format("GLFW Error: {} {}", error, description));
    }

    Application::Application(int width, int height, const char *name) : m_width(width), m_height(height) {
        if (name) {
            m_name = name;
        }

        Logger::get_instance().set_log_file("engine.log");
        Logger::get_instance().enable_console_logger(true);

        // Retrieve font path and size from config
        Config::load_config(std::string(SOURCE_DIR_PATH) + "/config.json");
    }

    void Application::initialize() {
        glfwSetErrorCallback(glfw_error_callback);

        if (!glfwInit()) {
            LOG_FATAL("Application: Failed to initialize GLFW");
        }

        std::string title = std::string(m_name) + " " + std::string(m_version);

        // --- Load Backend Description from Config ---
        BackendDescriptor backendDesc;
        if (!BackendDescriptor::LoadFromConfig(backendDesc)) {
            LOG_FATAL("Application: Failed to load valid backend description from config.");
            glfwTerminate();
        }
        // --------------------------------------------

        // --- Set Window Hints based on Backend Description ---
        if (backendDesc.type == RendererType::Vulkan) {
            if (!glfwVulkanSupported()) {
                LOG_FATAL("Application: Vulkan selected but not supported by GLFW/System.");
                glfwTerminate();
            }
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        } else if (backendDesc.type == RendererType::OpenGL) {
            // Use values from the description struct
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, backendDesc.opengl.majorVersion);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, backendDesc.opengl.minorVersion);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        } else {
            LOG_FATAL(fmt::format("Application: Unsupported renderer type ({}) configured.", static_cast<int>(backendDesc.type)));
            glfwTerminate();
        }


        m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);
        if (!m_window) {
            glfwTerminate();
            LOG_FATAL("Application: Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1); //Enable vsync

        // Initialize
        m_module_manager = std::make_unique<ModuleManager>();
        m_ui_manager = std::make_unique<UIManager>();
        m_renderer = RendererFactory::CreateRenderer(backendDesc);
        m_input_manager = std::make_unique<InputManager>();

        m_renderer->initialize(m_window);
        m_input_manager->initialize(m_window);
        m_ui_manager->initialize(m_window, m_ui_manager->getDpi());

        CurrentContext context;
        context.scene = &m_scene;
        context.dispatcher = &m_dispatcher;
        context.renderer = m_renderer.get();
        context.ui_manager = m_ui_manager.get();
        context.input_manager = m_input_manager.get();

        m_module_manager->initializeModules(context);

        m_lastFrameTime = (float) glfwGetTime();
    }

    void Application::run() {
        m_renderer->setClearColor(0.3, 0.5, 0.7, 1.0);
        while (!glfwWindowShouldClose(m_window)) {
            float currentTime = (float) glfwGetTime();
            float deltaTime = currentTime - m_lastFrameTime;
            m_lastFrameTime = currentTime;

            glfwPollEvents();

            m_input_manager->update();

            update(deltaTime);

            render();

            glfwSwapBuffers(m_window);
        }
    }

    void Application::update(float deltaTime) {
        m_module_manager->updateModules(deltaTime);
    }

    void Application::render() {
        m_renderer->render(m_scene);

        m_module_manager->renderModules();

        m_ui_manager->beginFrame();
        m_module_manager->renderUIModules();
        m_ui_manager->renderMainMenuBar(m_dispatcher);
        m_ui_manager->renderModuleUI(m_dispatcher);
        m_ui_manager->endFrame();
    }

    void Application::shutdown() {

        if(m_ui_manager){
            m_ui_manager->shutdown();
        }

        if(m_renderer){
            m_renderer->shutdown();
        }

        if(m_module_manager){
            m_module_manager->shutdownModules();
        }

        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        glfwTerminate();
        LOG_INFO(fmt::format("Application: Shutdown complete"));
    }
}