//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_POOLHANDLE_H
#define ENGINE25_POOLHANDLE_H

#include <limits>
#include <cassert>

namespace Bcg {
    template<typename T>
    class Pool;

    template<typename T>
    class PoolHandle {
    public:
        PoolHandle() : pool(nullptr), idx(std::numeric_limits<size_t>::max()) {

        }

        // Copy constructor
        PoolHandle(const PoolHandle &other) : pool(other.pool), idx(other.idx) {
            if (other.pool && idx < pool->properties.Size()) {
                assert(other.pool != nullptr);
                assert(pool != nullptr);

                pool->IncrementRefCount(idx);
            }
        }

        // Move constructor
        PoolHandle(PoolHandle &&other) noexcept : pool(other.pool), idx(other.idx) {
            other.pool = nullptr;
            other.idx = std::numeric_limits<size_t>::max();
        }

        [[nodiscard]] size_t GetIndex() const {
            return idx;
        }

        PoolHandle &operator=(const PoolHandle &other) {
            if (this != &other) {
                // Decrement current reference
                if (pool && idx < pool->properties.Size()) {
                    pool->DecrementRefCount(idx);
                }

                // Copy new reference
                pool = other.pool;
                idx = other.idx;
                if (pool && idx < pool->properties.Size()) {
                    pool->IncrementRefCount(idx);
                }
            }
            return *this;
        }

        PoolHandle &operator=(PoolHandle &&other) noexcept {
            if (this != &other) {
                // Decrement current reference
                if (pool && idx < pool->properties.Size()) {
                    pool->DecrementRefCount(idx);
                }

                // Move new reference
                pool = other.pool;
                idx = other.idx;
                other.pool = nullptr;
                other.idx = std::numeric_limits<size_t>::max();
            }
            return *this;
        }

        ~PoolHandle() {
            if (pool && idx < pool->properties.Size()) {
                pool->DecrementRefCount(idx);
            }
        }

        [[nodiscard]] bool IsValid() const {
            if (!pool || idx >= pool->properties.Size()) {
                return false;
            }
            size_t ref = pool->ref_count[idx];
            return ref > 0;
        }

        [[nodiscard]] size_t GetReferenceCount() const {
            return pool->ref_count[idx];
        }

        operator T &() {
            assert(IsValid() && "Dereferencing invalid PoolHandle!");
            return pool->objects[idx];
        }

        operator const T &() const {
            assert(IsValid() && "Dereferencing invalid PoolHandle!");
            return pool->objects[idx];
        }

        T &operator*() {
            assert(IsValid() && "Dereferencing invalid PoolHandle!");
            return pool->objects[idx];
        }

        const T &operator*() const {
            assert(IsValid() && "Dereferencing invalid PoolHandle!");
            return pool->objects[idx];
        }

        T *operator->() {
            assert(IsValid() && "Dereferencing invalid PoolHandle!");
            return &pool->objects[idx];
        }

        const T *operator->() const {
            assert(IsValid() && "Dereferencing invalid PoolHandle!");
            return &pool->objects[idx];
        }

        bool operator==(const PoolHandle &other) const {
            return pool == other.pool && idx == other.idx;
        }

        bool operator!=(const PoolHandle &other) const {
            return !(*this == other);
        }

    protected:
        friend class Pool<T>;

        PoolHandle(Pool<T> *pool, size_t idx) : pool(pool), idx(idx) {
            if (pool && idx < pool->properties.Size()) {
                pool->IncrementRefCount(idx);
            }
        }

        Pool<T> *pool;
        size_t idx;
    };
}

#endif //ENGINE25_POOLHANDLE_H
