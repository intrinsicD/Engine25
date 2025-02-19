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

        void connect_events() override;

        void disconnect_events() override;

        void on_initialize(const Events::Initialize &event) override;

        void on_startup(const Events::Startup &event) override;

        void on_synchronize(const Events::Synchronize &event) override;

        void on_shutdown(const Events::Shutdown &event) override;

        void InitGfx();
    };
}

#endif //ENGINE25_SLANGSHADERMODULE_H
