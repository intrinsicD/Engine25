//
// Created by alex on 2/11/25.
//

#ifndef GRAPHBELLMANFORD_H
#define GRAPHBELLMANFORD_H

#include "Graph.h"

namespace Bcg {
    class BellmanFord {
    public:
        explicit BellmanFord(Graph &graph);

        bool compute(const Vertex &source);

        bool compute(const std::vector<Vertex> &sources);

        bool has_negative_cycle() const;

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        void clear_custom_edge_weights();

        EdgeProperty<Real> edge_weights;
        VertexProperty<Real> vertex_distances;
        VertexProperty<Halfedge> vertex_predecessors;
        bool negative_cycle_found;

    private:
        void clear();

        Graph &graph;
    };
}

#endif //GRAPHBELLMANFORD_H
