//
// Created by alex on 02.04.25.
//

#include "Application.h"
#include "Logger.h"
#include "ConfigFile.h"
#include "GLFW/glfw3.h"
#include "RendererFactory.h"
#include "entt/entity/registry.hpp"
#include "entt/signal/dispatcher.hpp"

namespace Bcg {

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
        m_module_manager = std::make_unique<ModuleManager>();
        m_platform = std::make_unique<Platform>();
        m_ui_manager = std::make_unique<UIManager>();
        //Read WindowDesc and BackendDesc from config

        m_app_context.windowDesc.width = Config::get_int("window.width");
        m_app_context.windowDesc.height = Config::get_int("window.height");
        m_app_context.windowDesc.title = Config::get_string("window.title");

        m_app_context.backend.type = Config::get_string("backend.type");
        m_app_context.backend.opengl.majorVersion = Config::get_int("backend.opengl.majorVersion");
        m_app_context.backend.opengl.minorVersion = Config::get_int("backend.opengl.minorVersion");
        m_app_context.backend.opengl.profile = Config::get_string("backend.opengl.profile") == "core" ? 0 : 1;
        m_app_context.backend.vulkan.majorVersion = Config::get_int("backend.vulkan.majorVersion");
        m_app_context.backend.vulkan.minorVersion = Config::get_int("backend.vulkan.minorVersion");
        m_app_context.backend.vsync = Config::get_int("backend.vsync");

        m_platform->initialize(&m_app_context);
        m_renderer = RendererFactory::CreateRenderer(m_app_context.backend);
        m_renderer->initialize(&m_app_context);
        m_ui_manager->initialize(&m_app_context);

        m_module_manager->initializeModules(&m_app_context);

        m_lastFrameTime = (float) glfwGetTime();
    }

    void Application::run() {
        while (!glfwWindowShouldClose(m_window)) {
            float currentTime = (float) glfwGetTime();
            float deltaTime = currentTime - m_lastFrameTime;
            m_lastFrameTime = currentTime;

            m_platform->pollEvents();

            update(deltaTime);

            render();
        }
    }

    void Application::update(float deltaTime) {
        m_module_manager->updateModules(deltaTime);
    }

    void Application::render() {
        auto commandBuffer = m_renderer->acquireCommandBuffer();
        auto swapChain = m_renderer->acquireSwapchain();
        auto renderPasses = m_renderer->acquireRenderPasses();
        m_renderer->begin(commandBuffer);
        for (auto *renderPass: renderPasses) {
            renderPass->execute(commandBuffer);
        }
        m_renderer->end(commandBuffer);
        m_renderer->submit(commandBuffer);
        m_renderer->present(swapChain);
    }

    void Application::shutdown() {

        if (m_ui_manager) {
            m_ui_manager->shutdown();
        }

        if (m_renderer) {
            m_renderer->shutdown();
        }

        if (m_module_manager) {
            m_module_manager->shutdownModules();
        }

        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }

        if (m_platform) {
            m_platform->shutdown();
        }
        LOG_INFO(fmt::format("Application: Shutdown complete"));
    }

    ApplicationContext *Application::getContext() {
        return &m_app_context;
    }
}