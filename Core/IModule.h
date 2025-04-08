//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_IMODULE_H
#define ENGINE25_IMODULE_H

#include "ApplicationContext.h" // Include context definition

namespace Bcg {
    class IModule {
    public:
        // Constructor taking mandatory name/version
        IModule(const char *name, const char *version) : m_name(name), m_version(version), m_initialized(false) {}

        virtual ~IModule() = default; // Ensure virtual destructor

        [[nodiscard]] const char *getName() const { return m_name; }

        [[nodiscard]] const char *getVersion() const { return m_version; }

        [[nodiscard]] bool isInitialized() const { return m_initialized; }

        // Core lifecycle methods
        virtual bool initialize(ApplicationContext *context) {
            if (!context) return false;
            m_app_context = context;
            m_initialized = true; // Base class sets initialized flag
            return true;
        }

        virtual void shutdown() {
            m_initialized = false; // Clear flag on shutdown
            m_app_context = nullptr; // Clear context pointer
        }

        // Optional lifecycle methods (can be overridden if needed)
        virtual void update(float /*deltaTime*/) { /* Default no-op */ };

        virtual void render() { /* Default no-op */ };

        virtual void renderUI() { /* Default no-op */ };

        // Optional event connection (can be overridden if needed)
        virtual void connectDispatcher() { /* Default no-op */ };

        virtual void disconnectDispatcher() { /* Default no-op */ };

    protected:
        // No need for setContext, initialization handles it.
        ApplicationContext *getApplicationContext() { return m_app_context; }

        const ApplicationContext *getApplicationContext() const { return m_app_context; }

        const char *m_name;
        const char *m_version;
        bool m_initialized;
        ApplicationContext *m_app_context = nullptr; // Store context pointer
    };
}

#endif //ENGINE25_IMODULE_H