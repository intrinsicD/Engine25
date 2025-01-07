//
// Created by alex on 1/7/25.
//

#ifndef MESH_H
#define MESH_H

#include "GeometricProperties.h"

namespace Bcg {
    class PointCloud {
    public:
        size_t num_deleted_vertices = 0;
        VertexProperty<bool> deleted_vertices;
        Vertices vertices;

        inline size_t n_vertices() const { return vertices.size() - num_deleted_vertices; }
    };

    class Graph {
    public:
        Vertices vertices;
        Halfedges halfedges;
        Edges edges;
    };

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
