//
// Created by alex on 06.12.24.
//

#ifndef ENGINE25_CACHE_H
#define ENGINE25_CACHE_H

#include <unordered_map>

namespace Bcg{
    template<typename Key, typename Value>
    using Cache = std::unordered_map<Key, Value>;
}

#endif //ENGINE25_CACHE_H
