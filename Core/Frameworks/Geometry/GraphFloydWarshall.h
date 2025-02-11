//
// Created by alex on 2/11/25.
//

#ifndef GRAPHFLOYDWARSHALL_H
#define GRAPHFLOYDWARSHALL_H

#include "Graph.h"

namespace Bcg {
    // Floyd-Warshall: Computes all-pairs shortest paths.
    class FloydWarshall {
    public:
        explicit FloydWarshall(Graph &graph);

        void compute();

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        void clear_custom_edge_weights();

        EdgeProperty<Real> edge_weights;
        Matrix<Real, -1, -1> vertex_vertex_distances;
        Matrix<Halfedge, -1, -1> vertex_vertex_predecessors;

    private:
        void clear();
        Graph &graph;
    };
}

#endif //GRAPHFLOYDWARSHALL_H
