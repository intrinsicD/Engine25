//
// Created by alex on 13.12.24.
//

#include "SlangShaderModule.h"
#include "Engine.h"
#include "GraphicsApi.h"
#include "LoggingMacros.h"
#include <slang.h>
#include <slang-gfx.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

namespace Bcg{
    SlangShaderModule::SlangShaderModule() : Module("SlangShaderModule", "0.1") {

    }

    void SlangShaderModule::connect_events() {
        Engine::GetDispatcher().sink<Events::Initialize>().connect<&SlangShaderModule::OnInitialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().connect<&SlangShaderModule::OnStartup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().connect<&SlangShaderModule::OnSynchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().connect<&SlangShaderModule::OnShutdown>(*this);
        Module::ConnectEvents();
    }

    void SlangShaderModule::disconnect_events() {
        Engine::GetDispatcher().sink<Events::Initialize>().disconnect<&SlangShaderModule::OnInitialize>(this);
        Engine::GetDispatcher().sink<Events::Startup>().disconnect<&SlangShaderModule::OnStartup>(this);
        Engine::GetDispatcher().sink<Events::Synchronize>().disconnect<&SlangShaderModule::OnSynchronize>(this);
        Engine::GetDispatcher().sink<Events::Shutdown>().disconnect<&SlangShaderModule::OnShutdown>(this);
        Module::DisconnectEvents();
    }

    struct SlangContext{
        SlangSession *session;
    };

    void SlangShaderModule::on_initialize(const Events::Initialize &event) {
        Module::OnInitialize(event);



        Graphics::Device &device = Engine::GetContext().get<Graphics::Device>();
        Graphics::BackendType backend = device.GetBackendType();
        SlangSourceLanguage sourceLanguage = SLANG_SOURCE_LANGUAGE_SPIRV; // default

        switch (backend) {
            case Graphics::BackendType::Vulkan:
                sourceLanguage = SLANG_SOURCE_LANGUAGE_SPIRV;
                break;
            case Graphics::BackendType::OpenGL:
                sourceLanguage = SLANG_SOURCE_LANGUAGE_GLSL;
                break;
            case Graphics::BackendType::Direct3D12:
                sourceLanguage = SLANG_SOURCE_LANGUAGE_HLSL;
                break;
            case Graphics::BackendType::Metal:
                sourceLanguage = SLANG_SOURCE_LANGUAGE_METAL; // If supported
                break;
            default:
                // Handle unknown backend (error or default to something sensible)
                break;
        }
    }

    void SlangShaderModule::on_startup(const Events::Startup &event) {
        Module::OnStartup(event);
    }

    void SlangShaderModule::on_synchronize(const Events::Synchronize &event) {
        Module::OnSynchronize(event);
    }

    void SlangShaderModule::on_shutdown(const Events::Shutdown &event) {
        Module::OnShutdown(event);
    }

    using namespace gfx;

    struct MyDebugCallback : public IDebugCallback
    {
        virtual SLANG_NO_THROW void SLANG_MCALL handleMessage(
            DebugMessageType type,
            DebugMessageSource source,
            const char* message) override
        {
          LOG_ERROR(fmt::format("Slang error: %s", message));
        }
    }gCallback;

    void SlangShaderModule::InitGfx(){
        gfxEnableDebugLayer();
        gfxSetDebugCallback(&gCallback);

        IDevice::Desc deviceDesc = {};
        IDevice* gDevice = nullptr;
        gfxCreateDevice(&deviceDesc, &gDevice);
    }
}