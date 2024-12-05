//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_COMMAND_H
#define ENGINE25_COMMAND_H

#include <functional>

namespace Bcg {
    class AbstractCommand {
    public:
        AbstractCommand() = default;

        virtual ~AbstractCommand() = default;

        virtual void Execute() const = 0;
    };

    class Task : public AbstractCommand {
    public:
        explicit Task(std::function<void()> task) : AbstractCommand(), task(std::move(task)) {}

        ~Task() override = default;

        void Execute() const override {
            if (task) {
                task();
            }
        }

        std::function<void()> task;
    };
}

#endif //ENGINE25_COMMAND_H
