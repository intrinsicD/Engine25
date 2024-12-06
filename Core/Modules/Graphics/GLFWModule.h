//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_GLFWMODULE_H
#define ENGINE25_GLFWMODULE_H

#include "Module.h"
#include "Command.h"
#include "glm/glm.hpp"

namespace Bcg {
    class GLFWModule : public Module {
    public:
        GLFWModule();

        ~GLFWModule() override = default;

        void ConnectEvents() override;

        void DisconnectEvents() override;

        void OnInitialize(const Events::Initialize &event) override;

        void OnStartup(const Events::Startup &event) override;

        void OnSynchronize(const Events::Synchronize &event) override;

        void OnShutdown(const Events::Shutdown &event) override;

        static GLFWwindow *GetCurrentWindow();

        static glm::vec2 GetCursorPosition(GLFWwindow *window);

        static float GetDpiScaling();
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
