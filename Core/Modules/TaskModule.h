//
// Created by alex on 3/4/25.
//

#ifndef TASKMODULE_H
#define TASKMODULE_H

#include "Module.h"

namespace Bcg {
    class TaskModule : public Module {
    public:
        TaskModule();

        ~TaskModule() override = default;

        void connect_events() override;

        void disconnect_events() override;

        void on_initialize(const Events::Initialize &event) override;

        void on_startup(const Events::Startup &event) override;

        void on_synchronize(const Events::Synchronize &event) override;

        void on_shutdown(const Events::Shutdown &event) override;
    };
}
#endif //TASKMODULE_H
