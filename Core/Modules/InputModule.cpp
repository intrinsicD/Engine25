//
// Created by alex on 06.12.24.
//

#include "InputModule.h"
#include "Engine.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Logger.h"
#include "MainLoop.h"
#include "GLFW/glfw3.h"

namespace Bcg {
    InputModule::InputModule() : Module("InputModule", "0.1") {
    }

    void InputModule::connect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().connect<&InputModule::on_initialize>(*this);
        Engine::get_dispatcher().sink<Events::Startup>().connect<&InputModule::on_startup>(*this);
        Engine::get_dispatcher().sink<Events::Synchronize>().connect<&InputModule::on_synchronize>(*this);
        Engine::get_dispatcher().sink<Events::Shutdown>().connect<&InputModule::on_shutdown>(*this);
        Module::connect_events();
    }

    void InputModule::disconnect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().disconnect<&InputModule::on_initialize>(*this);
        Engine::get_dispatcher().sink<Events::Startup>().disconnect<&InputModule::on_startup>(*this);
        Engine::get_dispatcher().sink<Events::Synchronize>().disconnect<&InputModule::on_synchronize>(*this);
        Engine::get_dispatcher().sink<Events::Shutdown>().disconnect<&InputModule::on_shutdown>(*this);
        Module::disconnect_events();
    }

    void InputModule::on_initialize(const Events::Initialize &event) {
        Module::on_initialize(event);
        Engine::get_context().emplace<Mouse>();
        Engine::get_context().emplace<Keyboard>();
        Engine::get_context().emplace<KeyboardCallbacks>();
    }

    void InputModule::on_startup(const Events::Startup &event) {
        Module::on_startup(event);
    }

    void InputModule::on_synchronize(const Events::Synchronize &event) {
        Module::on_synchronize(event);
        auto &loop = Engine::get_context().get<MainLoop>();

        loop.end_gui.Next().AddCommand(std::make_shared<Task>([]() {
            auto &mouse = Engine::get_context().get<Mouse>();
            mouse.state = Mouse::State::Idle;
            mouse.wheel.scroll_offset = {0, 0};
        }));
    }

    void InputModule::on_shutdown(const Events::Shutdown &event) {
        Module::on_shutdown(event);
    }

    static bool enabled_key_logging = false;
    static bool enabled_mouse_logging = false;

    void InputModule::enable_key_logging(){
        enabled_key_logging = true;
    }

    void InputModule::disable_key_logging(){
        enabled_key_logging = false;
    }

    void InputModule::enable_mouse_logging(){
        enabled_mouse_logging = true;
    }

    void InputModule::disable_mouse_logging(){
        enabled_mouse_logging = false;
    }

    void InputModule::on_key(const Events::Key &event) {
        if(enabled_key_logging){
            LOG_INFO(fmt::format("InputModule::on_key: Key: {}, Scancode: {}, Action: {}, Mode: {}",
                                 event.key,event.scancode,event.action, event.mode));
        }

        auto &keyboard = Engine::get_context().get<Keyboard>();

        if(event.action == GLFW_PRESS){
            keyboard.PressKey(MapGlfwKey(event.key));
        } else if(event.action == GLFW_RELEASE){
            keyboard.ReleaseKey(MapGlfwKey(event.key));
        }
    }

    void InputModule::on_mouse_cursor(const Events::MouseCursor &event) {
        if(enabled_mouse_logging){
            LOG_INFO(fmt::format("InputModule::on_mouse_cursor: Xpos: {}, Ypos: {}", event.xpos, event.ypos));
        }

        auto &mouse = Engine::get_context().get<Mouse>();
        mouse.cursor_position = {event.xpos, event.ypos};
        if (mouse.any_pressed()) {
            mouse.state = Mouse::State::Drag;
        } else {
            mouse.state = Mouse::State::Move;
        }
    }

    void InputModule::on_mouse_button(const Events::MouseButton &event) {
        if(enabled_mouse_logging){
            LOG_INFO(fmt::format("InputModule::on_mouse_button: Button: {}, Action: {}, Mods: {}", event.button, event.action,
                                 event.mods));
        }

        auto &mouse = Engine::get_context().get<Mouse>();
        if (event.button == GLFW_MOUSE_BUTTON_LEFT) {
            if (event.action == GLFW_PRESS) {
                mouse.left.state = Mouse::Button::State::Press;
                mouse.left.last_press = mouse.cursor_position;
            } else if (event.action == GLFW_RELEASE) {
                mouse.left.state = Mouse::Button::State::Release;
                mouse.left.last_release = mouse.cursor_position;
            }
        } else if (event.button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (event.action == GLFW_PRESS) {
                mouse.right.state = Mouse::Button::State::Press;
                mouse.right.last_press = mouse.cursor_position;
            } else if (event.action == GLFW_RELEASE) {
                mouse.right.state = Mouse::Button::State::Release;
                mouse.right.last_release = mouse.cursor_position;
            }
        } else if (event.button == GLFW_MOUSE_BUTTON_MIDDLE) {
            if (event.action == GLFW_PRESS) {
                mouse.wheel.state = Mouse::Wheel::State::Press;
                mouse.wheel.last_press = mouse.cursor_position;
            } else if (event.action == GLFW_RELEASE) {
                mouse.wheel.state = Mouse::Wheel::State::Release;
                mouse.wheel.last_release = mouse.cursor_position;
            }
        }

        if (!mouse.any_pressed() && mouse.state == Mouse::State::Drag) {
            mouse.state = Mouse::State::Move;
        }
    }

    void InputModule::on_mouse_scroll(const Events::MouseScroll &event) {
        if(enabled_mouse_logging){
            LOG_INFO(fmt::format("InputModule::on_mouse_scroll: Xoffset: {}, Yoffset: {}", event.xoffset, event.yoffset));
        }

        auto &mouse = Engine::get_context().get<Mouse>();
        mouse.wheel.scroll_offset = {event.xoffset, event.yoffset};
        mouse.wheel.state = Mouse::Wheel::State::Scroll;
    }
}