//
// Created by alex on 04.12.24.
//

#include "Engine.h"
#include "LoggingMacros.h"
#include "MainLoop.h"
#include "EngineEvents.h"
#include "RenderingModule.h"
#include "GLFWModule.h"
#include "GuiModule.h"
#include "InputModule.h"
#include "ConfigFile.h"

namespace Bcg {
    static Scene scene;
    static Dispatcher dispatcher;

    void Engine::Init() {
        //TODO
        Logger::GetInstance().SetLogFile("engine.log");
        Logger::GetInstance().SetLogLevel(Logger::Level::TODO);
        Logger::GetInstance().EnableConsoleLogger(true);

        // Retrieve font path and size from config
        Config::LoadConfig(std::string(SOURCE_DIR_PATH) + "/config.json");

        LOG_INFO(fmt::format("{}::Init: version {}", name, version));

        auto &loop = GetContext().emplace<MainLoop>();
        auto &renderer = GetContext().emplace<RenderingModule>();
        auto &glfw = GetContext().emplace<GLFWModule>();
        auto &gui = GetContext().emplace<GuiModule>();
        auto &input = GetContext().emplace<InputModule>();

        renderer.SetBackend({RenderingModule::Backend::Type::OpenGL, "OpenGL", "4.6"});

        renderer.ConnectEvents();
        glfw.ConnectEvents();
        gui.ConnectEvents();
        input.ConnectEvents();

        dispatcher.trigger<Events::Initialize>();
        LogThisFrame().Execute();
    }

    void Engine::Startup() {
        //TODO
        LOG_INFO(fmt::format("{}::Startup", name));
        dispatcher.trigger<Events::Startup>();
    }

    void Engine::Run() {
        //TODO
        LOG_INFO(fmt::format("{}::Run", name));
        GetContext().get<MainLoop>().Run();
    }

    void Engine::Shutdown() {
        //TODO
        LOG_INFO(fmt::format("{}::Shutdown", name));
        dispatcher.trigger<Events::Shutdown>();
    }

    Scene &Engine::GetScene() {
        return scene;
    }

    Context &Engine::GetContext() {
        return scene.ctx();
    }

    Dispatcher &Engine::GetDispatcher() {
        return dispatcher;
    }
}