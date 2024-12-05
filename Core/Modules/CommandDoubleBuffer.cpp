//
// Created by alex on 05.12.24.
//

#include "CommandDoubleBuffer.h"

namespace Bcg {
    CommandBuffer &DoubleCommandBuffer::Current() const {
        return *p_current;
    }

    CommandBuffer &DoubleCommandBuffer::Next() const {
        return *p_next;
    }

    void DoubleCommandBuffer::SwapBuffers() {
        std::swap(p_current, p_next);
    }

    void DoubleCommandBuffer::Handle() {
        p_current->Execute_commands();
        p_current->Clear();
        SwapBuffers();
    }
}