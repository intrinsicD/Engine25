//
// Created by alex on 05.12.24.
//

#include "Module.h"
#include "LoggingMacros.h"

namespace Bcg {
    Module::Module(const char *name, const char *version) : name(name), version(version) {}

    void Module::connect_events() {
        LOG_INFO(fmt::format("Connected to events: {} {}", name, version));
    }

    void Module::disconnect_events() {
        LOG_INFO(fmt::format("Disconnected from events: {} {}", name, version));
    }

    void Module::on_initialize(const Events::Initialize &event) {
        LOG_INFO(fmt::format("Initialize {} {}", name, version));
    }

    void Module::on_startup(const Events::Startup &event) {
        LOG_INFO(fmt::format("Startup {} {}", name, version));
    }

    void Module::on_synchronize(const Events::Synchronize &event) {

    }

    void Module::on_shutdown(const Events::Shutdown &event) {
        LOG_INFO(fmt::format("Shutdown {} {}", name, version));
    }
}