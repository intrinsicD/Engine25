//
// Created by dieckman on 13.03.25.
//

#ifndef SCENEMODULE_H
#define SCENEMODULE_H

#include "Module.h"

namespace Bcg {
    class SceneModule : public Module {
    public:
        SceneModule();

        ~SceneModule() override = default;

        void connect_events() override;

        void disconnect_events() override;

        void on_initialize(const Events::Initialize &event) override;

        void on_startup(const Events::Startup &event) override;

        void on_synchronize(const Events::Synchronize &event) override;

        void on_shutdown(const Events::Shutdown &event) override;

        void on_drop_file(const Events::Drop &event);


    };
}
#endif //SCENEMODULE_H
