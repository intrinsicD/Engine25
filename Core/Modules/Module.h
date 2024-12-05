//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_MODULE_H
#define ENGINE25_MODULE_H

#include "EngineEvents.h"

namespace Bcg {
    class Module {
    public:
        Module(const char *name, const char *version);

        virtual ~Module() = default;

        virtual void connect_events();

        virtual void disconnect_events();

        virtual void on_initialize(const Events::Initialize &event);

        virtual void on_startup(const Events::Startup &event);

        virtual void on_synchronize(const Events::Synchronize &event);

        virtual void on_shutdown(const Events::Shutdown &event);

        const char *name;
        const char *version;
    };
}

#endif //ENGINE25_MODULE_H
