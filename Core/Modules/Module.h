//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_MODULE_H
#define ENGINE25_MODULE_H

#include "EngineEvents.h"
#include <string>

namespace Bcg {
    class Module {
    public:
        Module(std::string name, std::string version);

        virtual ~Module() = default;

        virtual void ConnectEvents();

        virtual void DisconnectEvents();

        virtual void OnInitialize(const Events::Initialize &event);

        virtual void OnStartup(const Events::Startup &event);

        virtual void OnSynchronize(const Events::Synchronize &event);

        virtual void OnShutdown(const Events::Shutdown &event);

        std::string name;
        std::string version;
    };
}

#endif //ENGINE25_MODULE_H
