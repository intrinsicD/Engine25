//
// Created by alex on 05.12.24.
//

#include "GuiModule.h"

namespace Bcg{
    GuiModule::GuiModule() : Module("GuiModule", "0.1") {}

    void GuiModule::on_initialize(const Events::Initialize &event) {
        Module::on_initialize(event);
    }

    void GuiModule::on_startup(const Events::Startup &event) {
        Module::on_startup(event);
    }

    void GuiModule::on_synchronize(const Events::Synchronize &event) {

    }

    void GuiModule::on_shutdown(const Events::Shutdown &event) {
        Module::on_shutdown(event);
    }
}