//
// Created by alex on 2/11/25.
//

#ifndef GRAPHPRIM_H
#define GRAPHPRIM_H

#include "Graph.h"

namespace Bcg {
    // Prim: Computes a minimum spanning tree (MST) starting from a given vertex.
    class Prim {
    public:
        explicit Prim(Graph &graph);

        void compute(const Vertex &source);

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        void clear_custom_edge_weights();

        EdgeProperty<Real> edge_weights;
        VertexProperty<Halfedge> vertex_predecessors; // Predecessor halfedge for each vertex in the MST to the source.

    private:
        void clear();

        Graph &graph;
    };
}

#endif //GRAPHPRIM_H
