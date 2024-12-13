//
// Created by alex on 13.12.24.
//

#include "SlangShaderModule.h"
#include "Engine.h"
#include "GraphicsApi.h"

namespace Bcg{
    SlangShaderModule::SlangShaderModule() : Module("SlangShaderModule", "0.1") {

    }

    void SlangShaderModule::ConnectEvents() {
        Engine::GetDispatcher().sink<Events::Initialize>().connect<&SlangShaderModule::OnInitialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().connect<&SlangShaderModule::OnStartup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().connect<&SlangShaderModule::OnSynchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().connect<&SlangShaderModule::OnShutdown>(*this);
        Module::ConnectEvents();
    }

    void SlangShaderModule::DisconnectEvents() {
        Engine::GetDispatcher().sink<Events::Initialize>().disconnect<&SlangShaderModule::OnInitialize>(this);
        Engine::GetDispatcher().sink<Events::Startup>().disconnect<&SlangShaderModule::OnStartup>(this);
        Engine::GetDispatcher().sink<Events::Synchronize>().disconnect<&SlangShaderModule::OnSynchronize>(this);
        Engine::GetDispatcher().sink<Events::Shutdown>().disconnect<&SlangShaderModule::OnShutdown>(this);
        Module::DisconnectEvents();
    }

    void SlangShaderModule::OnInitialize(const Events::Initialize &event) {
        Module::OnInitialize(event);

        Graphics::Device &device = Engine::GetContext().get<Graphics::Device>();
        device.
    }

    void SlangShaderModule::OnStartup(const Events::Startup &event) {
        Module::OnStartup(event);
    }

    void SlangShaderModule::OnSynchronize(const Events::Synchronize &event) {
        Module::OnSynchronize(event);
    }

    void SlangShaderModule::OnShutdown(const Events::Shutdown &event) {
        Module::OnShutdown(event);
    }
}