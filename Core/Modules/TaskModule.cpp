//
// Created by alex on 3/4/25.
//

#include "TaskModule.h"
#include "Engine.h"
#include "JobSystem.h"
#include "ConfigFile.h"

namespace Bcg {
    TaskModule::TaskModule() : Module("TaskModule", "0.1") {

    }

    void TaskModule::connect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().connect<&TaskModule::on_initialize>(this);
        Engine::get_dispatcher().sink<Events::Shutdown>().connect<&TaskModule::on_shutdown>(this);
        Module::connect_events();
    }

    void TaskModule::disconnect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().disconnect<&TaskModule::on_initialize>(this);
        Engine::get_dispatcher().sink<Events::Shutdown>().disconnect<&TaskModule::on_shutdown>(this);
        Module::disconnect_events();
    }

    void TaskModule::on_initialize(const Events::Initialize &event) {
        auto &jobs = Engine::get_context().emplace<JobSystem>(Config::get_int("jobs.max_threads"));
        Module::on_initialize(event);
    }

    void TaskModule::on_startup(const Events::Startup &event) {
        Module::on_startup(event);
    }

    void TaskModule::on_synchronize(const Events::Synchronize &event) {
        Module::on_synchronize(event);
    }

    void TaskModule::on_shutdown(const Events::Shutdown &event) {
        auto &jobs = Engine::get_context().get<JobSystem>();
        jobs.wait();
        Module::on_shutdown(event);
    }
}