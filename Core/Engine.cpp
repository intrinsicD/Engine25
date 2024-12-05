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
#include "ConfigFile.h"
#include "ConfigFile.h"

namespace Bcg {
    static Scene scene;
    static Dispatcher dispatcher;

    void Engine::Init() {
        //TODO
        Logger::GetInstance().SetLogFile("engine.log");
        Logger::GetInstance().SetLogLevel(Logger::Level::Info);
        Logger::GetInstance().EnableConsoleLogger(true);

        // Retrieve font path and size from config
        Config::LoadConfig(std::string(SOURCE_DIR_PATH) + "/config.json");

        LOG_INFO(fmt::format("Init {} version {}", name, version));
        auto &loop = GetContext().emplace<MainLoop>();
        auto &renderer = GetContext().emplace<RenderingModule>();
        auto &glfw = GetContext().emplace<GLFWModule>();
        auto &gui = GetContext().emplace<GuiModule>();

        renderer.SetBackend({RenderingModule::Backend::Type::OpenGL, "OpenGL", "4.6"});

        renderer.ConnectEvents();
        glfw.ConnectEvents();
        gui.ConnectEvents();

        dispatcher.trigger<Events::Initialize>();

 /*       loop.end.Next().AddCommand(std::make_shared<Task>([&loop]() {
            loop.Stop();
            LOG_INFO("MainLoop stopped from end command");
        }));*/
    }

    void Engine::Startup() {
        //TODO
        dispatcher.trigger<Events::Startup>();
        LOG_INFO(fmt::format("Startup {} version {}", name, version));
    }

    void Engine::Run() {
        //TODO
        LOG_INFO(fmt::format("Run {} version {}", name, version));
        GetContext().get<MainLoop>().Run();
    }

    void Engine::Shutdown() {
        //TODO
        dispatcher.trigger<Events::Shutdown>();
        LOG_INFO(fmt::format("Shutdown {} version {}", name, version));
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