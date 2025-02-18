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
    static Dispatcher dispatcher;

    void Engine::Init() {
        //TODO
        Logger::GetInstance().SetLogFile("engine.log");
        Logger::GetInstance().SetLogLevel(Logger::Level::TODO);
        Logger::GetInstance().EnableConsoleLogger(true);

        // Retrieve font path and size from config
        Config::LoadConfig(std::string(SOURCE_DIR_PATH) + "/config.json");

        LOG_INFO(fmt::format("{}::Init: version {}", name, version));

        auto &jobs = GetContext().emplace<JobSystem>(Config::GetInt("jobs.max_threads"));
        auto &taskGraph = GetContext().emplace<TaskGraph>();
        auto &renderGraph = GetContext().emplace<Graphics::RenderGraph>();
        auto &loop = GetContext().emplace<MainLoop>();
        auto &renderer = GetContext().emplace<Graphics::RenderingModule>();
        auto &glfw = GetContext().emplace<Graphics::GLFWModule>();
        auto &gui = GetContext().emplace<Graphics::GuiModule>();
        auto &input = GetContext().emplace<InputModule>();

        renderer.SetBackend({Graphics::RenderingModule::Backend::Type::OpenGL, "OpenGL", "4.6"});

        renderer.ConnectEvents();
        glfw.ConnectEvents();
        gui.ConnectEvents();
        input.ConnectEvents();

        dispatcher.trigger<Events::Initialize>();
        Graphics::LogThisFrame().Execute();
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
        auto &jobs = GetContext().get<JobSystem>();
        jobs.Wait();
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

    TaskGraph &Engine::GetTaskGraph() {
        return GetContext().get<TaskGraph>();
    }

    Graphics::RenderGraph &Engine::GetRenderGraph() {
        return GetContext().get<Graphics::RenderGraph>();
    }
}