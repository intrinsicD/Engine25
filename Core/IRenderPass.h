//
// Created by alex on 03.04.25.
//

#ifndef ENGINE25_IRENDERPASS_H
#define ENGINE25_IRENDERPASS_H

namespace Bcg{
    class IRenderPass {
    public:
        virtual ~IRenderPass() = default;

        virtual void initialize() = 0;

        virtual void render() = 0;

        virtual void shutdown() = 0;
    };
}

#endif //ENGINE25_IRENDERPASS_H
