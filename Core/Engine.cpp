//
// Created by alex on 04.12.24.
//

#include "Engine.h"
#include "Logger.h"
#include "MainLoop.h"
#include "EngineEvents.h"
#include "RenderingModule.h"
#include "GLFWModule.h"
#include "GuiModule.h"
#include "InputModule.h"
#include "ConfigFile.h"
#include "JobSystem.h"

namespace Bcg {
    static Scene scene;
    static Scene context;
    static Dispatcher dispatcher;

    void Engine::init() {
        //TODO
        Logger::get_instance().set_log_file("engine.log");
        Logger::get_instance().enable_console_logger(true);

        // Retrieve font path and size from config
        Config::load_config(std::string(SOURCE_DIR_PATH) + "/config.json");

        LOG_INFO(fmt::format("{}::Init: version {}", name, version));

        auto &jobs = get_context().emplace<JobSystem>(Config::get_int("jobs.max_threads"));
        auto &taskGraph = get_context().emplace<TaskGraph>();
        auto &renderGraph = get_context().emplace<Graphics::RenderGraph>();
        auto &loop = get_context().emplace<MainLoop>();
        auto &renderer = get_context().emplace<Graphics::RenderingModule>();
        auto &glfw = get_context().emplace<Graphics::GLFWModule>();
        auto &gui = get_context().emplace<Graphics::GuiModule>();
        auto &input = get_context().emplace<InputModule>();

        renderer.SetBackend({Graphics::RenderingModule::Backend::Type::OpenGL, "OpenGL", "4.6"});

        renderer.connect_events();
        glfw.connect_events();
        gui.connect_events();
        input.connect_events();

        dispatcher.trigger<Events::Initialize>();
        Graphics::LogThisFrame().Execute();
    }

    void Engine::startup() {
        //TODO
        LOG_INFO(fmt::format("{}::Startup", name));
        dispatcher.trigger<Events::Startup>();
    }

    void Engine::run() {
        //TODO
        LOG_INFO(fmt::format("{}::Run", name));
        get_context().get<MainLoop>().Run();
    }

    void Engine::shutdown() {
        //TODO
        LOG_INFO(fmt::format("{}::Shutdown", name));
        dispatcher.trigger<Events::Shutdown>();
        auto &jobs = get_context().get<JobSystem>();
        jobs.Wait();
    }

    Scene &Engine::get_scene() {
        return scene;
    }

    Context &Engine::get_context() {
        return context.ctx();
    }

    Dispatcher &Engine::get_dispatcher() {
        return dispatcher;
    }

    TaskGraph &Engine::get_task_graph() {
        return get_context().get<TaskGraph>();
    }

    Graphics::RenderGraph &Engine::get_render_graph() {
        return get_context().get<Graphics::RenderGraph>();
    }
}