//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_COMMANDBUFFER_H
#define ENGINE25_COMMANDBUFFER_H

#include <vector>
#include <memory>
#include <mutex>
#include "Command.h"

namespace Bcg {
    class CommandBuffer {
    public:
        CommandBuffer &AddCommand(std::shared_ptr<AbstractCommand> command);

        CommandBuffer &AddCommands(std::vector<std::shared_ptr<AbstractCommand>> &commands);

        void Execute_commands();

        void Clear();

        auto &Commands() { return commands; }

        const auto &Commands() const { return commands; }

    private:
        std::mutex mutex;
        std::vector<std::shared_ptr<AbstractCommand> > commands;
    };
}

#endif //ENGINE25_COMMANDBUFFER_H
