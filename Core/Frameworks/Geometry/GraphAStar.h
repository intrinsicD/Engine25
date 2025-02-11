//
// Created by alex on 2/11/25.
//

#ifndef GRAPHASTAR_H
#define GRAPHASTAR_H

#include "Graph.h"

namespace Bcg {
    // A*: An informed search algorithm that uses a heuristic function.
    class AStar {
    public:
        explicit AStar(Graph &graph);

        void compute(const Vertex &source, const Vertex &target);

        void set_heuristic(std::function<Real(const Vertex &)> heuristic);

        void clear_heuristic();

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        void clear_custom_edge_weights();

        EdgeProperty<Real> edge_weights;
        VertexProperty<Real> vertex_distances;
        VertexProperty<Halfedge> vertex_predecessors;
        std::function<Real(const Vertex &)> heuristic;

    private:
        void clear();

        Graph &graph;
    };
}

#endif //GRAPHASTAR_H
