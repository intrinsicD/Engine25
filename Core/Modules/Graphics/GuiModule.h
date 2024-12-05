//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_GUIMODULE_H
#define ENGINE25_GUIMODULE_H

#include "Module.h"

namespace Bcg {
    class GuiModule : public Module {
    public:
        GuiModule();

        ~GuiModule() override = default;

        void on_initialize(const Events::Initialize &event) override;

        void on_startup(const Events::Startup &event) override;

        void on_synchronize(const Events::Synchronize &event) override;

        void on_shutdown(const Events::Shutdown &event) override;
    };
}

#endif //ENGINE25_GUIMODULE_H
