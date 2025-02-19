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

        void connect_events() override;

        void disconnect_events() override;

        void on_initialize(const Events::Initialize &event) override;

        void on_startup(const Events::Startup &event) override;

        void on_synchronize(const Events::Synchronize &event) override;

        void on_shutdown(const Events::Shutdown &event) override;

        static void enable_key_logging();

        static void disable_key_logging();

        static void enable_mouse_logging();

        static void disable_mouse_logging();

        static void on_key(const Events::Key &event);

        static void on_mouse_cursor(const Events::MouseCursor &event);

        static void on_mouse_button(const Events::MouseButton &event);

        static void on_mouse_scroll(const Events::MouseScroll &event);
    };
}
#endif //ENGINE25_INPUTMODULE_H
