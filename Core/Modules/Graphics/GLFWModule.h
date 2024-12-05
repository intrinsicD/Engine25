//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_GLFWMODULE_H
#define ENGINE25_GLFWMODULE_H

#include "Module.h"
#include "Command.h"

namespace Bcg {
    class GLFWModule : public Module {
    public:
        GLFWModule();

        ~GLFWModule() override = default;

        void connect_events() override;

        void disconnect_events() override;

        void on_initialize(const Events::Initialize &event) override;

        void on_startup(const Events::Startup &event) override;

        void on_synchronize(const Events::Synchronize &event) override;

        void on_shutdown(const Events::Shutdown &event) override;
    };

    struct PollEvents : public AbstractCommand {
        PollEvents() : AbstractCommand() {}

        void Execute() const override;
    };

    struct SwapBuffers : public AbstractCommand {
        SwapBuffers() : AbstractCommand() {}

        void Execute() const override;
    };
}

#endif //ENGINE25_GLFWMODULE_H