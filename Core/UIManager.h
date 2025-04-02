//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_UIMANAGER_H
#define ENGINE25_UIMANAGER_H

#include <string>
#include "entt/signal/fwd.hpp"

struct GLFWwindow;

namespace Bcg{
    class UIManager{
    public:
        UIManager() = default;

        ~UIManager() = default;

        void initialize(GLFWwindow *m_window, float dpi);

        void shutdown();

        void beginFrame();

        void endFrame();

        // Add methods later to register UI rendering callbacks from modules
        void renderMainMenuBar(entt::dispatcher &dispatcher); // Example

        void renderModuleUI(entt::dispatcher &dispatcher);    // Example placeholder

        float getDpi();

        bool loadFont(const std::string &fontPath, float fontSize);

    private:
        bool m_initialized = false;
        bool m_demo_window = false;
        bool m_demo_menu = true;
    };
}

#endif //ENGINE25_UIMANAGER_H
