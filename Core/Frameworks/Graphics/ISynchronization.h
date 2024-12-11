//
// Created by alex on 12/12/24.
//

#ifndef ISYNCHRONIZATION_H
#define ISYNCHRONIZATION_H

namespace Bch::Graphics {
    class IFence {
    public:
        virtual ~IFence() = default;

        virtual void wait(uint64_t timeout = UINT64_MAX) = 0;

        virtual bool isSignaled() const = 0;
    };

    class ISemaphore {
    public:
        virtual ~ISemaphore() = default;
    };
}
#endif //ISYNCHRONIZATION_H
