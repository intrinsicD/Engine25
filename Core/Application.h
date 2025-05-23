//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_APPLICATION_H
#define ENGINE25_APPLICATION_H

#include <memory>
#include "entt/entity/registry.hpp"
#include "entt/signal/dispatcher.hpp"
#include "VulkanRenderer.h"
#include "UIManager.h"
#include "ModuleManager.h"
#include "ApplicationContext.h"
#include "SlangShaderModule.h"
#include "Platform.h"

struct GLFWwindow;

namespace Bcg {


    class Application {
    public:

        explicit Application(int width = 1280, int height = 720, const char *name = nullptr);

        virtual ~Application();

        void initialize();

        void run();

    private:
        void update(float delta_time);

        void render();

        void shutdown();

        ApplicationContext *getContext();

        const char *m_name = "Application";
        const char *m_version = "0.0.1";
        ApplicationContext m_app_context;

        GLFWwindow *m_window = nullptr;
        int m_width;
        int m_height;
        float m_lastFrameTime = 0.0f;

        std::unique_ptr<UIManager> m_ui_manager;
        std::unique_ptr<VulkanRenderer> m_renderer;
        std::unique_ptr<ModuleManager> m_module_manager;
        std::unique_ptr<Platform> m_platform;
        std::unique_ptr<SlangShaderModule> m_shader_module;

        entt::dispatcher m_dispatcher;
        entt::registry m_scene;
    };
}

#endif //ENGINE25_APPLICATION_H
