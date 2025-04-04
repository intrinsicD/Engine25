//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_IMODULE_H
#define ENGINE25_IMODULE_H

#include "Events/EngineEvents.h"
#include "ApplicationContext.h"

namespace Bcg {
    class IModule {
    public:
        IModule(const char *name, const char *version) : m_name(name), m_version(version) {};

        virtual ~IModule() = default;

        [[nodiscard]] const char *getName() const {
            return m_name;
        }

        [[nodiscard]] const char *getVersion() const {
            return m_version;
        }

        [[nodiscard]] bool isInitialized() const {
            return m_app_context != nullptr;
        }

        virtual bool initialize(ApplicationContext *context) = 0;

        virtual void shutdown() = 0;

        virtual void update(float deltaTime) = 0;

        virtual void render() = 0;

        virtual void renderUI() = 0;

        virtual void connectDispatcher() = 0;

        virtual void disconnectDispatcher() = 0;

    protected:
        const char *m_name = "Module";
        const char *m_version = "0.0.1";

        bool setContext(ApplicationContext *context) {
            m_app_context = context;
            return m_app_context!= nullptr;
        }

        ApplicationContext *m_app_context = nullptr;
    };
}

#endif //ENGINE25_IMODULE_H
