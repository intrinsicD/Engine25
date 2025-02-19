//
// Created by alex on 05.12.24.
//

#include "Module.h"
#include "Logger.h"

namespace Bcg {
    Module::Module(std::string name, std::string version) : name(std::move(name)), version(std::move(version)) {
    }

    void Module::connect_events() {
        LOG_INFO(fmt::format("{}::connect_events", name));
    }

    void Module::disconnect_events() {
        LOG_INFO(fmt::format("{}::disconnect_events", name));
    }

    void Module::on_initialize(const Events::Initialize &event) {
        LOG_INFO(fmt::format("{}::on_initialize: version {}", name, version));
    }

    void Module::on_startup(const Events::Startup &event) {
        LOG_INFO(fmt::format("{}::on_startup", name));
    }

    void Module::on_synchronize(const Events::Synchronize &event) {
        LOG_FRAME(fmt::format("{}::on_synchronize", name));
    }

    void Module::on_shutdown(const Events::Shutdown &event) {
        LOG_INFO(fmt::format("{}::on_shutdown", name));
    }
}
