//
// Created by alex on 04.12.24.
//

#ifndef ENGINE25_COMMANDDOUBLEBUFFER_H
#define ENGINE25_COMMANDDOUBLEBUFFER_H

#include "CommandBuffer.h"

namespace Bcg {
    class DoubleCommandBuffer {
    public:
        DoubleCommandBuffer() = default;

        CommandBuffer &Current() const;

        CommandBuffer &Next() const;

        void SwapBuffers();

        void Handle();

    private:
        CommandBuffer a, b;

        CommandBuffer *p_current = &a;
        CommandBuffer *p_next = &b;
    };
}

#endif //ENGINE25_COMMANDDOUBLEBUFFER_H
