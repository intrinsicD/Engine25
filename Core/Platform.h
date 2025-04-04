//
// Created by alex on 04.04.25.
//

#ifndef ENGINE25_PLATFORM_H
#define ENGINE25_PLATFORM_H

#include <functional>
#include "IModule.h"
#include "IRenderer.h"

namespace Bcg {

    class Platform : public IModule {
    public:
        using ResizeCallback = std::function<void(int, int)>;
        using KeyCallback = std::function<void(int, int, int, int)>;
        using MouseButtonCallback = std::function<void(int, int, int)>;
        using MouseCallback = std::function<void(double, double)>;
        using ScrollCallback = std::function<void(double, double)>;
        using CharCallback = std::function<void(unsigned int)>;
        using DropCallback = std::function<void(int, const char **)>;
        using WindowCloseCallback = std::function<void()>;

        Platform();

        ~Platform() override = default;

        bool initialize(ApplicationContext *context) override;

        void shutdown() override;

        void pollEvents();

        bool shouldClose() const;

        double getTime() const;

        ResizeCallback m_resizeCallback = nullptr;
        KeyCallback m_keyCallback = nullptr;
        MouseButtonCallback m_mouseButtonCallback = nullptr;
        MouseCallback m_mouseCallback = nullptr;
        ScrollCallback m_scrollCallback = nullptr;
        CharCallback m_charCallback = nullptr;
        DropCallback m_dropCallback = nullptr;
        WindowCloseCallback m_windowCloseCallback = nullptr;
    private:

        void update(float deltaTime) override {

        }

        void render() override {

        }

        void renderUI() override {

        }

        void connectDispatcher() override {

        }

        void disconnectDispatcher() override {

        }

    };

} // namespace Bcg

#endif //ENGINE25_PLATFORM_H
