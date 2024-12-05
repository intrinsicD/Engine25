//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_POOL_H
#define ENGINE25_POOL_H

#include "Properties.h"
#include <queue>

namespace Bcg {
    template<typename T>
    class PoolHandle;


    template<typename T>
    class Pool {
    public:
        Pool(const char *name);

        virtual ~Pool() = default;

        const char *GetName() const { return name; }

        PoolHandle<T> Create();

        template<typename U>
        PoolHandle<T> Create(U &&obj);

        void Destroy(const PoolHandle<T> &handle);

        PropertyContainer &GetProperties() { return properties; }

        Property<size_t> &GetRefCount() { return ref_count; }

        Property<T> &GetObjects() { return objects; }

    protected:
        friend struct PoolHandle<T>;
        const char *name;
        PropertyContainer properties;
        Property<size_t> ref_count;
        Property<T> objects;
        std::queue<size_t> free_list;
    };

    template<typename T>
    Pool<T>::Pool(const char *name) : name(name), ref_count(properties.get_or_add<size_t>("ref_count", 0)),
                                      objects(properties.get_or_add<T>("objects")) {
    }

    template<typename T>
    PoolHandle<T> Pool<T>::Create() {
        size_t idx;
        if (free_list.empty()) {
            properties.push_back();
            idx = properties.size() - 1;
        } else {
            idx = free_list.front();
            free_list.pop();
        }
        return {this, idx};
    }

    template<typename T>
    template<typename U>
    PoolHandle<T> Pool<T>::Create(U &&obj) {
        PoolHandle<T> handle = Create();
        properties.get<T>("objects")[handle.idx] = std::forward<U>(obj);
        return handle;
    }

    template<typename T>
    void Pool<T>::Destroy(const PoolHandle<T> &handle) {
        if (handle.pool && handle.idx < handle.pool->properties.Size()) {
            size_t &ref_count = handle.pool->ref_count[handle.idx];
            if (ref_count > 0) {
                --ref_count;
            }
            if (ref_count == 0) {
                free_list.push(handle.idx);
            }
        }
    }
}

#endif //ENGINE25_POOL_H
