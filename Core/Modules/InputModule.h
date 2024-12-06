//
// Created by alex on 06.12.24.
//

#ifndef ENGINE25_INPUTMODULE_H
#define ENGINE25_INPUTMODULE_H

#include "Module.h"

namespace Bcg {
    class InputModule : public Module {
    public:
        InputModule();

        ~InputModule() override = default;

        void ConnectEvents() override;

        void DisconnectEvents() override;

        void OnInitialize(const Events::Initialize &event) override;

        void OnStartup(const Events::Startup &event) override;

        void OnSynchronize(const Events::Synchronize &event) override;

        void OnShutdown(const Events::Shutdown &event) override;

        static void EnableKeyLogging();

        static void DisableKeyLogging();

        static void EnableMouseLogging();

        static void DisableMouseLogging();

        static void OnKey(const Events::Key &event);

        static void OnMouseCursor(const Events::MouseCursor &event);

        static void OnMouseButton(const Events::MouseButton &event);

        static void OnMouseScroll(const Events::MouseScroll &event);
    };
}
#endif //ENGINE25_INPUTMODULE_H
