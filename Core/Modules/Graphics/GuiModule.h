//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_GUIMODULE_H
#define ENGINE25_GUIMODULE_H

#include "Module.h"
#include "Command.h"

namespace Bcg {
    class GuiModule : public Module {
    public:
        GuiModule();

        ~GuiModule() override = default;

        void OnInitialize(const Events::Initialize &event) override;

        void OnStartup(const Events::Startup &event) override;

        void OnSynchronize(const Events::Synchronize &event) override;

        void OnShutdown(const Events::Shutdown &event) override;

        static void LoadFonts(std::string font_path, float font_size);
    };

    class InitializeImGui : public AbstractCommand {
    public:
        InitializeImGui() : AbstractCommand() {
        }

        void Execute() const override;
    };

    //TODO add these commands via synchronize to the main loop

    class BeginGui : public AbstractCommand {
    public:
        BeginGui() : AbstractCommand() {
        }

        void Execute() const override;
    };

    class EndGui : public AbstractCommand {
    public:
        EndGui() : AbstractCommand() {
        }

        void Execute() const override;
    };
}

#endif //ENGINE25_GUIMODULE_H
