//
// Created by alex on 05.12.24.
//

#include "CommandBuffer.h"

namespace Bcg{
    CommandBuffer &CommandBuffer::AddCommand(std::shared_ptr<AbstractCommand> command) {
        std::scoped_lock lock(mutex);
        commands.emplace_back(command);
        return *this;
    }

    CommandBuffer &CommandBuffer::AddCommands(std::vector<std::shared_ptr<AbstractCommand>> &commands_) {
        std::scoped_lock lock(mutex);
        commands.insert(commands.end(), commands_.begin(), commands_.end());
        return *this;
    }

    void CommandBuffer::Execute_commands() {
        std::scoped_lock lock(mutex);
        for (auto &command: commands) {
            command->Execute();
        }
    }

    void CommandBuffer::Clear() {
        std::scoped_lock lock(mutex);
        commands.clear();
    }
}