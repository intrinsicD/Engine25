//
// Created by alex on 1/7/25.
//

#ifndef MESH_H
#define MESH_H

#include "GeometricProperties.h"

namespace Bcg {
    class Mesh {
    public:
        Vertices vertices;
        Halfedges halfedges;
        Edges edges;
        Faces faces;
        Tets tets;
    };
}

#endif //MESH_H
