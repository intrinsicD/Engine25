//
// Created by alex on 11.03.25.
//

#ifndef ENGINE25_HIERARCHY_H
#define ENGINE25_HIERARCHY_H

#include "entt/entity/entity.hpp"
#include <set>

namespace Bcg{
    template<typename IdType, class ComponentType>
    struct Hierarchy{
        using id_type = IdType;
        using component_type = ComponentType;

        id_type parent;
        std::set<IdType> children;
    };

    template<class ComponentType>
    struct Hierarchy<entt::entity, ComponentType>{
        using id_type = entt::entity;
        using component_type = ComponentType;

        entt::entity parent;
        std::set<entt::entity> children;
    };
}

#endif //ENGINE25_HIERARCHY_H
