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
#include "TaskModule.h"
#include "MeshAssetModule.h"
#include "ConfigFile.h"

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

        auto &loop = get_context().emplace<MainLoop>();
        auto &renderer = get_context().emplace<Graphics::RenderingModule>();
        auto &glfw = get_context().emplace<Graphics::GLFWModule>();
        auto &gui = get_context().emplace<Graphics::GuiModule>();
        auto &tasks = get_context().emplace<TaskModule>();
        auto &input = get_context().emplace<InputModule>();
        auto &mesh_assets = get_context().emplace<MeshAssetModule>();

        renderer.SetBackend({Graphics::RenderingModule::Backend::Type::OpenGL, "OpenGL", "4.6"});

        renderer.connect_events();
        glfw.connect_events();
        gui.connect_events();
        input.connect_events();
        tasks.connect_events();
        mesh_assets.connect_events();

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
}