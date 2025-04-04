//
// Created by alex on 02.04.25.
//

#include "ModuleManager.h"
#include "Logger.h"

namespace Bcg{

    void ModuleManager::registerModule(std::unique_ptr<IModule> module) {
        if (!module) {
            LOG_WARN(fmt::format("ModuleManager: Tried to register a null module."));
            return;
        }
        if (m_initialized) {
            LOG_WARN(fmt::format("ModuleManager: Tried to register module '{}' after initialization.", module->getName() ? module->getName() : "Unnamed"));
            return;
        }
        LOG_INFO(fmt::format("ModuleManager: Registering module: {}", module->getName() ? module->getName() : "Unnamed"));
        m_modules.emplace_back(std::move(module));
    }

    void ModuleManager::initializeModules(ApplicationContext *context) {
        if (m_initialized) {
            LOG_WARN(fmt::format("ModuleManager: Modules already initialized."));
            return;
        }
        if (m_shutdown) {
            LOG_ERROR(fmt::format("ModuleManager: Cannot initialize modules after shutdown."));
            return;
        }
        LOG_INFO(fmt::format("ModuleManager: Initializing modules..."));
        for (const auto& mod_ptr : m_modules) {
            if (!mod_ptr) continue;
            const char* name = mod_ptr->getName() ? mod_ptr->getName() : "Unnamed";
            if(!mod_ptr->initialize(context)){
                LOG_ERROR(fmt::format("ModuleManager: Failed to initialize module '{}'", name));
                continue;
            }
            LOG_INFO(fmt::format("ModuleManager: Module '{}' initialized successfully.", name));
        }
        m_initialized = true;
        LOG_INFO(fmt::format("ModuleManager: All modules initialized."));
    }

    void ModuleManager::updateModules(float deltaTime) {
        if (!m_initialized || m_shutdown) return;

        for (const auto& mod_ptr : m_modules) {
            if (mod_ptr) {
                mod_ptr->update(deltaTime);
            }
        }
    }

    void ModuleManager::renderModules() {
        if (!m_initialized || m_shutdown) return;

        for (const auto& mod_ptr : m_modules) {
            if (mod_ptr) {
                mod_ptr->render();
            }
        }
    }

    void ModuleManager::renderUIModules() {
        if (!m_initialized || m_shutdown) return;

        for (const auto& mod_ptr : m_modules) {
            if (mod_ptr) {
                mod_ptr->renderUI();
            }
        }
    }

    void ModuleManager::shutdownModules() {
        if (!m_initialized || m_shutdown) {
            return;
        }

        LOG_INFO(fmt::format("ModuleManager: Shutting down modules..."));

        // Shutdown in reverse order of initialization/registration
        // Use reverse iterators
        for (auto it = m_modules.rbegin(); it != m_modules.rend(); ++it) {
            const auto& mod_ptr = *it;
            if (!mod_ptr) continue;
            mod_ptr->shutdown();
        }

        m_modules.clear();
        m_shutdown = true;
        m_initialized = false;
        LOG_INFO(fmt::format("ModuleManager: All modules shut down successfully."));
    }
}