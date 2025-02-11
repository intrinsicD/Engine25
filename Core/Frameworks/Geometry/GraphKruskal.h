//
// Created by alex on 2/11/25.
//

#ifndef GRAPHKRUSKAL_H
#define GRAPHKRUSKAL_H

#include "Graph.h"

namespace Bcg {
    // Kruskal: Computes a minimum spanning tree by processing edges in sorted order.
    class Kruskal {
    public:
        explicit Kruskal(Graph &graph);

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

#endif //GRAPHKRUSKAL_H
