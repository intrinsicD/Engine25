//
// Created by alex on 05.12.24.
//

#include "RenderingModule.h"
#include "Engine.h"
#include "MainLoop.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "LoggingMacros.h"
#include "WindowComponent.h"

namespace Bcg {
    RenderingModule::RenderingModule() : Module("RenderingModule", "0.1") {

    }

    void RenderingModule::ConnectEvents() {
        Engine::GetDispatcher().sink<Events::Initialize>().connect<&RenderingModule::OnInitialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().connect<&RenderingModule::OnStartup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().connect<&RenderingModule::OnSynchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().connect<&RenderingModule::OnShutdown>(*this);
        Module::ConnectEvents();
    }

    void RenderingModule::DisconnectEvents() {
        Engine::GetDispatcher().sink<Events::Initialize>().disconnect<&RenderingModule::OnInitialize>(this);
        Engine::GetDispatcher().sink<Events::Startup>().disconnect<&RenderingModule::OnStartup>(this);
        Engine::GetDispatcher().sink<Events::Synchronize>().disconnect<&RenderingModule::OnSynchronize>(this);
        Engine::GetDispatcher().sink<Events::Shutdown>().disconnect<&RenderingModule::OnShutdown>(this);
        Module::DisconnectEvents();
    }

    void RenderingModule::OnInitialize(const Events::Initialize &event) {
        Module::OnInitialize(event);
        prepare_render.AddCommand(std::make_shared<ClearFrameBufferColorDepth>());
        auto &loop = Engine::GetContext().get<MainLoop>();
        loop.begin.Next().AddCommand(std::make_shared<InitializeOpenGL>());
        loop.begin.Next().AddCommand(std::make_shared<SetClearColor>(0.3f, 0.5f, 0.7f));
    }

    void RenderingModule::OnStartup(const Events::Startup &event) {
        Module::OnStartup(event);
    }

    void RenderingModule::OnSynchronize(const Events::Synchronize &event) {
        auto &loop = Engine::GetContext().get<MainLoop>();
        loop.prepare_render.Next().AddCommands(prepare_render.Commands());
        loop.end_render.Next().AddCommands(end_render.Commands());
    }

    void RenderingModule::OnShutdown(const Events::Shutdown &event) {
        Module::OnShutdown(event);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Commands
    //------------------------------------------------------------------------------------------------------------------

    void InitializeOpenGL::Execute() const {
        // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            LOG_FATAL("Failed to initialize OpenGL context");
        } else {
            LOG_INFO(fmt::format("Loaded OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version)));
        }
    }

    void SetClearColor::Execute() const {
        glClearColor(color[0], color[1], color[2], 1.0f);
        auto &window = Engine::GetContext().get<WindowComponent>();
        window->clear_color[0] = color[0];
        window->clear_color[1] = color[1];
        window->clear_color[2] = color[2];
    }

    void ClearFrameBuffer::Execute() const {
        glClear(component_mask);
    }

    void ClearFrameBufferColor::Execute() const {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void ClearFrameBufferColorDepth::Execute() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}