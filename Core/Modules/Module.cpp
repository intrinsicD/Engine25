//
// Created by alex on 05.12.24.
//

#include "Module.h"
#include "LoggingMacros.h"

namespace Bcg {
    Module::Module(std::string name, std::string version) : name(std::move(name)), version(std::move(version)) {
    }

    void Module::ConnectEvents() {
        LOG_INFO(fmt::format("{}::ConnectEvents", name));
    }

    void Module::DisconnectEvents() {
        LOG_INFO(fmt::format("{}::DisconnectEvents", name));
    }

    void Module::OnInitialize(const Events::Initialize &event) {
        LOG_INFO(fmt::format("{}::OnInitialize: version {}", name, version));
    }

    void Module::OnStartup(const Events::Startup &event) {
        LOG_INFO(fmt::format("{}::OnStartup", name));
    }

    void Module::OnSynchronize(const Events::Synchronize &event) {
        LOG_FRAME(fmt::format("{}::OnSynchronize", name));
    }

    void Module::OnShutdown(const Events::Shutdown &event) {
        LOG_INFO(fmt::format("{}::OnShutdown", name));
    }
}
