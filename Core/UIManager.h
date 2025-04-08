//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_UIMANAGER_H
#define ENGINE25_UIMANAGER_H

#include <string>
#include "entt/signal/fwd.hpp"
#include "IModule.h"

struct GLFWwindow;

namespace Bcg{
    class UIManager : public IModule{
    public:
        UIManager();

        ~UIManager() override = default;

        bool initialize(ApplicationContext *context) override;

        void shutdown() override;

        void beginFrame();

        void endFrame();

        void renderMainMenuBar(); // Example

        void renderModuleUI();    // Example placeholder

        float getDpi();

        bool loadFont(const std::string &fontPath, float fontSize);

        void update(float) override{

        }
    private:

        void render() override{

        }

        void renderUI() override{

        }

        void connectDispatcher() override{

        }

        void disconnectDispatcher() override{

        }

        bool m_demo_window = false;
        bool m_demo_menu = true;
    };
}

#endif //ENGINE25_UIMANAGER_H
