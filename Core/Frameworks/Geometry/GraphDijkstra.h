//
// Created by alex on 2/11/25.
//

#ifndef GRAPHDIJKSTRA_H
#define GRAPHDIJKSTRA_H

#include "Graph.h"

namespace Bcg {
    // Dijkstra: Computes shortest paths from a source (or multiple sources)
    // for graphs with non-negative edge weights.
    class Dijkstra {
    public:
        explicit Dijkstra(Graph &graph);

        void compute(const Vertex &source, const Vertex &sink = Vertex());

        void compute(const std::vector<Vertex> &sources, const Vertex &sink = Vertex());

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        void clear_custom_edge_weights();

        EdgeProperty<Real> edge_weights;
        VertexProperty<Real> vertex_distances;
        VertexProperty<Halfedge> vertex_predecessors;
    private:
        void clear();

        Graph &graph;
    };
}
#endif //GRAPHDIJKSTRA_H
