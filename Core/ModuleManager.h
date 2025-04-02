//
// Created by alex on 02.04.25.
//

#ifndef ENGINE25_MODULEMANAGER_H
#define ENGINE25_MODULEMANAGER_H

#include "IModule.h"
#include <memory>
#include <vector>

namespace Bcg{
    class ModuleManager{
    public:
        ModuleManager() = default;

        ~ModuleManager() = default;

        void registerModule(std::unique_ptr<IModule> module);

        void initializeModules(CurrentContext &context);

        void shutdownModules();

        void updateModules(float deltaTime);

        void renderModules();

        void renderUIModules();

    private:
        std::vector<std::unique_ptr<IModule>> m_modules;
        bool m_initialized = false;
        bool m_shutdown = false;
    };
}

#endif //ENGINE25_MODULEMANAGER_H
