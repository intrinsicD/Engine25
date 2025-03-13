//
// Created by dieckman on 13.03.25.
//


#include "SceneModule.h"
#include "Engine.h"

namespace Bcg {
    SceneModule::SceneModule() : Module("SceneModule", "0.1") {

    }

    void SceneModule::connect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().connect<&SceneModule::on_initialize>(this);
        Engine::get_dispatcher().sink<Events::Shutdown>().connect<&SceneModule::on_shutdown>(this);
        Module::connect_events();
    }

    void SceneModule::disconnect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().disconnect<&SceneModule::on_initialize>(this);
        Engine::get_dispatcher().sink<Events::Shutdown>().disconnect<&SceneModule::on_shutdown>(this);
        Module::disconnect_events();
    }

    void SceneModule::on_initialize(const Events::Initialize &event) {
        Module::on_initialize(event);
    }

    void SceneModule::on_startup(const Events::Startup &event) {
        Module::on_startup(event);
    }

    void SceneModule::on_synchronize(const Events::Synchronize &event) {
        Module::on_synchronize(event);
    }

    void SceneModule::on_shutdown(const Events::Shutdown &event) {
        Module::on_shutdown(event);
    }
}