//
// Created by alex on 13.12.24.
//

#ifndef ENGINE25_SLANGSHADERMODULE_H
#define ENGINE25_SLANGSHADERMODULE_H

#include "Module.h"

namespace Bcg{
    class SlangShaderModule : public Module{
    public:
        SlangShaderModule();

        ~SlangShaderModule() override = default;

        void ConnectEvents() override;

        void DisconnectEvents() override;

        void OnInitialize(const Events::Initialize &event) override;

        void OnStartup(const Events::Startup &event) override;

        void OnSynchronize(const Events::Synchronize &event) override;

        void OnShutdown(const Events::Shutdown &event) override;

        void InitGfx();
    };
}

#endif //ENGINE25_SLANGSHADERMODULE_H
