//
// Created by alex on 05.12.24.
//

#ifndef ENGINE25_POOLHANDLE_H
#define ENGINE25_POOLHANDLE_H

#include <limits>

namespace Bcg {
    template<typename T>
    class Pool;

    template<typename T>
    struct PoolHandle {
        PoolHandle() : pool(nullptr), idx(std::numeric_limits<size_t>::max()) {}

        // Copy constructor
        PoolHandle(const PoolHandle &other) : pool(other.pool), idx(other.idx) {
            if (other.pool && idx < pool->properties.size()) {
                ++pool->properties.template get<size_t>("ref_count")[idx];
            }
        }

        // Move constructor
        PoolHandle(PoolHandle &&other) noexcept : pool(other.pool), idx(other.idx) {
            other.pool = nullptr;
            other.idx = std::numeric_limits<size_t>::max();
        }

        size_t get_index() const {
            return idx;
        }

        PoolHandle& operator=(const PoolHandle& other) {
            if (this != &other) {
                // Decrement current reference
                if (pool && idx < pool->properties.size()) {
                    size_t &ref = pool->properties.template get<size_t>("ref_count")[idx];
                    if (ref > 0) {
                        --ref;
                        if (ref == 0) {
                            pool->free_list.push(idx);
                        }
                    }
                }

                // Copy new reference
                pool = other.pool;
                idx = other.idx;
                if (pool && idx < pool->properties.size()) {
                    ++pool->properties.template get<size_t>("ref_count")[idx];
                }
            }
            return *this;
        }

        PoolHandle& operator=(PoolHandle&& other) noexcept {
            if (this != &other) {
                // Decrement current reference
                if (pool && idx < pool->properties.size()) {
                    size_t &ref = pool->properties.template get<size_t>("ref_count")[idx];
                    if (ref > 0) {
                        --ref;
                        if (ref == 0) {
                            pool->free_list.push(idx);
                        }
                    }
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
            if (pool && idx < pool->properties.size()) {
                size_t &ref_count = pool->properties.template get<size_t>("ref_count")[idx];
                if (ref_count > 0) {
                    --ref_count;
                    if (ref_count == 0) {
                        pool->free_list.push(idx);
                    }
                }
            }
        }

        bool is_valid() const {
            return pool && idx < pool->properties.size() && get_ref_count() > 0;
        }

        size_t get_ref_count() const {
            return pool->properties.template get<size_t>("ref_count")[idx];
        }

        operator T &() {
            return pool->properties.template get<T>("objects")[idx];
        }

        operator const T &() const {
            return pool->properties.template get<T>("objects")[idx];
        }

        T &operator*() {
            return pool->properties.template get<T>("objects")[idx];
        }

        const T &operator*() const {
            return pool->properties.template get<T>("objects")[idx];
        }

        T *operator->() {
            return &pool->properties.template get<T>("objects")[idx];
        }

        const T *operator->() const {
            return &pool->properties.template get<T>("objects")[idx];
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
            if (pool && idx < pool->properties.size()) {
                ++pool->properties.template get_or_add<size_t>("ref_count", 0)[idx];
            }
        }

        Pool<T> *pool;
        size_t idx;
    };
}

#endif //ENGINE25_POOLHANDLE_H
