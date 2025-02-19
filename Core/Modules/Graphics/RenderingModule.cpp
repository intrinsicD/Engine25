//
// Created by alex on 05.12.24.
//

#include "RenderingModule.h"
#include "Engine.h"
#include "MainLoop.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Logger.h"
#include "WindowComponent.h"
#include <slang/slang.h>

namespace Bcg::Graphics {
    RenderingModule::RenderingModule() : Module("RenderingModule", "0.1"){

    }

    void RenderingModule::connect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().connect<&RenderingModule::on_initialize>(*this);
        Engine::get_dispatcher().sink<Events::Startup>().connect<&RenderingModule::on_startup>(*this);
        Engine::get_dispatcher().sink<Events::Synchronize>().connect<&RenderingModule::on_synchronize>(*this);
        Engine::get_dispatcher().sink<Events::Shutdown>().connect<&RenderingModule::on_shutdown>(*this);
        Module::connect_events();
    }

    void RenderingModule::disconnect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().disconnect<&RenderingModule::on_initialize>(this);
        Engine::get_dispatcher().sink<Events::Startup>().disconnect<&RenderingModule::on_startup>(this);
        Engine::get_dispatcher().sink<Events::Synchronize>().disconnect<&RenderingModule::on_synchronize>(this);
        Engine::get_dispatcher().sink<Events::Shutdown>().disconnect<&RenderingModule::on_shutdown>(this);
        Module::disconnect_events();
    }

    void RenderingModule::on_initialize(const Events::Initialize &event) {
        Module::on_initialize(event);
        prepare_render.AddCommand(std::make_shared<ClearFrameBufferColorDepth>());
        auto &loop = Engine::get_context().get<MainLoop>();
        loop.begin.Next().AddCommand(std::make_shared<InitializeOpenGL>());
        loop.begin.Next().AddCommand(std::make_shared<SetClearColor>(0.3f, 0.5f, 0.7f));
    }

    void RenderingModule::on_startup(const Events::Startup &event) {
        Module::on_startup(event);
        auto &window = Engine::get_context().get<WindowComponent>();
        auto &loop = Engine::get_context().get<MainLoop>();
        loop.begin.Next().AddCommand(std::make_shared<SetViewport>(0, 0, window->width, window->height));
    }

    void RenderingModule::on_synchronize(const Events::Synchronize &event) {
        Module::on_synchronize(event);
        auto &loop = Engine::get_context().get<MainLoop>();
        loop.prepare_render.Next().AddCommands(prepare_render.Commands());
        loop.end_render.Next().AddCommands(end_render.Commands());
    }

    void RenderingModule::on_shutdown(const Events::Shutdown &event) {
        Module::on_shutdown(event);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Commands
    //------------------------------------------------------------------------------------------------------------------

    void InitializeOpenGL::Execute() const {
        // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            LOG_FATAL("Command::InitializeOpenGL: Failed to initialize OpenGL context");
        } else {
            LOG_INFO(fmt::format("Command::InitializeOpenGL: Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version),
                                 GLAD_VERSION_MINOR(version)));
        }
    }

    void LogThisFrame::Execute() const {

        auto &loop = Engine::get_context().get<MainLoop>();
        loop.begin.Next().AddCommand(std::make_shared<Task>([&loop]() {
            LOG_WARN("Command::LogThisFrame: Enable frame logging");
            EnableFrameLogging(true);
            loop.begin.Next().AddCommand(std::make_shared<Task>([]() {
                EnableFrameLogging(false);
                LOG_WARN("Command::LogThisFrame: Disable frame logging");
            }));
        }));

    }

    void SetClearColor::Execute() const {
        glClearColor(color[0], color[1], color[2], 1.0f);
        auto &window = Engine::get_context().get<WindowComponent>();
        window->clear_color[0] = color[0];
        window->clear_color[1] = color[1];
        window->clear_color[2] = color[2];
        LOG_INFO(fmt::format("Command::SetClearColor: [{}, {}, {}]", color[0], color[1], color[2]));
    }

    void SetViewport::Execute() const {
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        LOG_INFO(fmt::format("Command::SetViewport: [{}, {}, {}, {}]", viewport[0], viewport[1], viewport[2],
                             viewport[3]));
    }

    void ClearFrameBuffer::Execute() const {
        glClear(component_mask);
        LOG_FRAME(fmt::format("Command::ClearFrameBuffer: bit mask: {}", component_mask));
    }

    void ClearFrameBufferColor::Execute() const {
        glClear(GL_COLOR_BUFFER_BIT);
        LOG_FRAME("Command::ClearFrameBufferColor");
    }

    void ClearFrameBufferColorDepth::Execute() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        LOG_FRAME("Command::ClearFrameBufferColorDepth");
    }
}