//
// Created by alex on 06.12.24.
//

#include "InputModule.h"
#include "Engine.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "LoggingMacros.h"
#include "MainLoop.h"
#include "GLFW/glfw3.h"

namespace Bcg {
    InputModule::InputModule() : Module("InputModule", "0.1") {
    }

    void InputModule::ConnectEvents() {
        Engine::GetDispatcher().sink<Events::Initialize>().connect<&InputModule::OnInitialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().connect<&InputModule::OnStartup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().connect<&InputModule::OnSynchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().connect<&InputModule::OnShutdown>(*this);
        Module::ConnectEvents();
    }

    void InputModule::DisconnectEvents() {
        Engine::GetDispatcher().sink<Events::Initialize>().disconnect<&InputModule::OnInitialize>(*this);
        Engine::GetDispatcher().sink<Events::Startup>().disconnect<&InputModule::OnStartup>(*this);
        Engine::GetDispatcher().sink<Events::Synchronize>().disconnect<&InputModule::OnSynchronize>(*this);
        Engine::GetDispatcher().sink<Events::Shutdown>().disconnect<&InputModule::OnShutdown>(*this);
        Module::DisconnectEvents();
    }

    void InputModule::OnInitialize(const Events::Initialize &event) {
        Module::OnInitialize(event);
        Engine::GetContext().emplace<Mouse>();
        Engine::GetContext().emplace<Keyboard>();
        Engine::GetContext().emplace<KeyboardCallbacks>();
    }

    void InputModule::OnStartup(const Events::Startup &event) {
        Module::OnStartup(event);
    }

    void InputModule::OnSynchronize(const Events::Synchronize &event) {
        Module::OnSynchronize(event);
        auto &loop = Engine::GetContext().get<MainLoop>();

        loop.end_gui.Next().AddCommand(std::make_shared<Task>([]() {
            auto &mouse = Engine::GetContext().get<Mouse>();
            mouse.state = Mouse::State::Idle;
            mouse.wheel.scroll_offset = {0, 0};
        }));
    }

    void InputModule::OnShutdown(const Events::Shutdown &event) {
        Module::OnShutdown(event);
    }

    static bool enabled_key_logging = false;
    static bool enabled_mouse_logging = false;

    void InputModule::EnableKeyLogging(){
        enabled_key_logging = true;
    }

    void InputModule::DisableKeyLogging(){
        enabled_key_logging = false;
    }

    void InputModule::EnableMouseLogging(){
        enabled_mouse_logging = true;
    }

    void InputModule::DisableMouseLogging(){
        enabled_mouse_logging = false;
    }

    void InputModule::OnKey(const Events::Key &event) {
        if(enabled_key_logging){
            LOG_INFO(fmt::format("InputModule::OnKey: Key: {}, Scancode: {}, Action: {}, Mode: {}",
                                 event.key,event.scancode,event.action, event.mode));
        }

        auto &keyboard = Engine::GetContext().get<Keyboard>();

        if(event.action == GLFW_PRESS){
            keyboard.PressKey(MapGlfwKey(event.key));
        } else if(event.action == GLFW_RELEASE){
            keyboard.ReleaseKey(MapGlfwKey(event.key));
        }
    }

    void InputModule::OnMouseCursor(const Events::MouseCursor &event) {
        if(enabled_mouse_logging){
            LOG_INFO(fmt::format("InputModule::OnMouseCursor: Xpos: {}, Ypos: {}", event.xpos, event.ypos));
        }

        auto &mouse = Engine::GetContext().get<Mouse>();
        mouse.cursor_position = {event.xpos, event.ypos};
        if (mouse.any_pressed()) {
            mouse.state = Mouse::State::Drag;
        } else {
            mouse.state = Mouse::State::Move;
        }
    }

    void InputModule::OnMouseButton(const Events::MouseButton &event) {
        if(enabled_mouse_logging){
            LOG_INFO(fmt::format("InputModule::OnMouseButton: Button: {}, Action: {}, Mods: {}", event.button, event.action,
                                 event.mods));
        }

        auto &mouse = Engine::GetContext().get<Mouse>();
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

    void InputModule::OnMouseScroll(const Events::MouseScroll &event) {
        if(enabled_mouse_logging){
            LOG_INFO(fmt::format("InputModule::OnMouseScroll: Xoffset: {}, Yoffset: {}", event.xoffset, event.yoffset));
        }

        auto &mouse = Engine::GetContext().get<Mouse>();
        mouse.wheel.scroll_offset = {event.xoffset, event.yoffset};
        mouse.wheel.state = Mouse::Wheel::State::Scroll;
    }
}