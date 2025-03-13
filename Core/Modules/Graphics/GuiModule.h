//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_GUIMODULE_H
#define ENGINE25_GUIMODULE_H

#include "Module.h"
#include "CommandBuffer.h"

namespace Bcg::Graphics {
    class GuiModule : public Module {
    public:
        GuiModule();

        ~GuiModule() override = default;

        void connect_events() override;

        void disconnect_events() override;

        void on_initialize(const Events::Initialize &event) override;

        void on_startup(const Events::Startup &event) override;

        void on_synchronize(const Events::Synchronize &event) override;

        void on_shutdown(const Events::Shutdown &event) override;

        static bool WantCaptureKeyboard();

        static bool WantCaptureMouse();

        static void LoadFonts(const std::string &font_path, float font_size);

        CommandBuffer active_gui_commands;
    };

    class InitializeImGui : public AbstractCommand {
    public:
        InitializeImGui() : AbstractCommand() {
        }

        void Execute() const override;
    };

    class BeginGui : public AbstractCommand {
    public:
        BeginGui() : AbstractCommand() {
        }

        void Execute() const override;
    };

    class RenderMenu : public AbstractCommand {
    public:
        RenderMenu() : AbstractCommand() {
        }

        void Execute() const override;
    };

    class RenderGui : public AbstractCommand {
    public:
        RenderGui() : AbstractCommand() {
        }

        void Execute() const override;
    };

    class MockMenu : public AbstractCommand {
    public:
        MockMenu() : AbstractCommand() {
        }

        void Execute() const override;
    };

    class EndGui : public AbstractCommand {
    public:
        EndGui() : AbstractCommand() {
        }

        void Execute() const override;
    };

    class AddGuiMenuEntry : public Task {
    public:
        explicit AddGuiMenuEntry(std::function<void()> task) : Task(std::move(task)) {

        }

        void Execute() const override{
            Task::Execute();
        }
    };

    class AddGuiWidget : public Task {
    public:
        explicit AddGuiWidget(std::function<void()> task) : Task(std::move(task)) {

        }

        void Execute() const override{
            Task::Execute();
        }

        bool active = true;
    };
}

#endif //ENGINE25_GUIMODULE_H
