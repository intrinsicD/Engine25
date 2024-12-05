//
// Created by alex on 05.12.24.
//

#include "Module.h"
#include "LoggingMacros.h"

namespace Bcg {
    Module::Module(std::string name, std::string version) : name(std::move(name)), version(std::move(version)) {
    }

    void Module::ConnectEvents() {
        LOG_INFO(fmt::format("Connected to events: {} {}", name, version));
    }

    void Module::DisconnectEvents() {
        LOG_INFO(fmt::format("Disconnected from events: {} {}", name, version));
    }

    void Module::OnInitialize(const Events::Initialize &event) {
        LOG_INFO(fmt::format("Initialize {} {}", name, version));
    }

    void Module::OnStartup(const Events::Startup &event) {
        LOG_INFO(fmt::format("Startup {} {}", name, version));
    }

    void Module::OnSynchronize(const Events::Synchronize &event) {
        //don't log this event since it is called every frame
    }

    void Module::OnShutdown(const Events::Shutdown &event) {
        LOG_INFO(fmt::format("Shutdown {} {}", name, version));
    }
}
