//
// Created by alex on 12/12/24.
//

#ifndef IQUERYOBJECT_H
#define IQUERYOBJECT_H

namespace Bcg::Graphics {
    class IQueryPool {
    public:
        virtual ~IQueryPool() = default;

        virtual void reset() = 0;

        virtual void beginQuery(ICommandBuffer *cmdBuffer, uint32_t queryIndex) = 0;

        virtual void endQuery(ICommandBuffer *cmdBuffer, uint32_t queryIndex) = 0;

        virtual void getResults(uint32_t startIndex, uint32_t queryCount, void *results) = 0;
    };
}
#endif //IQUERYOBJECT_H
