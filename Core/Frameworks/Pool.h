//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_POOL_H
#define ENGINE25_POOL_H

#include "Properties.h"
#include <queue>
#include <mutex>

namespace Bcg {
    template<typename T>
    class PoolHandle;


    template<typename T>
    class Pool {
    public:
        explicit Pool(const char *name);

        virtual ~Pool() = default;

        [[nodiscard]] const char *GetName() const { return name; }

        PoolHandle<T> CreateHandle();

        template<typename U>
        PoolHandle<T> CreateHandle(U &&obj);

        PropertyContainer &GetProperties() { return properties; }

        Property<size_t> &GetRefCount() { return ref_count; }

        Property<T> &GetObjects() { return objects; }

    protected:

        void IncrementRefCount(size_t idx);

        void DecrementRefCount(size_t idx);

        friend class PoolHandle<T>;
        const char *name;
        PropertyContainer properties;
        Property<size_t> ref_count;
        Property<T> objects;
        std::queue<size_t> free_list;
        std::mutex mutex;
    };

    template<typename T>
    Pool<T>::Pool(const char *name) : name(name), ref_count(properties.get_or_add<size_t>("ref_count", 0)),
                                      objects(properties.get_or_add<T>("objects")) {
    }

    template<typename T>
    PoolHandle<T> Pool<T>::CreateHandle() {
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
    PoolHandle<T> Pool<T>::CreateHandle(U &&obj) {
        PoolHandle<T> handle = CreateHandle();
        objects[handle.idx] = std::forward<U>(obj);
        return handle;
    }

    template<typename T>
    void Pool<T>::IncrementRefCount(size_t idx) {
        std::scoped_lock lock(mutex);
        assert(idx < properties.size());
        assert(ref_count);

        ++ref_count[idx];
    }

    template<typename T>
    void Pool<T>::DecrementRefCount(size_t idx) {
        std::scoped_lock lock(mutex);
        assert(idx < properties.size());
        assert(ref_count);

        size_t &ref = ref_count[idx];
        if (ref > 0) {
            --ref;
            if (ref == 0) {
                free_list.push(idx);
            }
        } else {
            assert(false && "Reference count underflow!");
        }
    }
}

#endif //ENGINE25_POOL_H
