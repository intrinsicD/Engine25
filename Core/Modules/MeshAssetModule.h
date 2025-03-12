//
// Created by alex on 11.03.25.
//

#ifndef ENGINE25_MESHASSETMODULE_H
#define ENGINE25_MESHASSETMODULE_H

#include "Module.h"

namespace Bcg {
    class MeshAssetModule : public Module {
    public:
        MeshAssetModule();

        ~MeshAssetModule() override = default;

        void connect_events() override;

        void disconnect_events() override;

        void on_initialize(const Events::Initialize &event) override;

        void on_startup(const Events::Startup &event) override;

        void on_synchronize(const Events::Synchronize &event) override;

        void on_shutdown(const Events::Shutdown &event) override;

        void on_drop_file(const Events::Drop &event);
    };
}

#endif //ENGINE25_MESHASSETMODULE_H
