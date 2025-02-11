//
// Created by alex on 2/11/25.
//

#ifndef GRAPHDIJKSTRA_H
#define GRAPHDIJKSTRA_H

#include "Graph.h"

namespace Bcg {
    /**
     * @brief Dijkstra: Computes shortest paths from a source (or multiple sources)
     * for graphs with non-negative edge weights.
     */
    class Dijkstra {
    public:
        /**
         * @brief Constructs a Dijkstra object for the given graph.
         * @param graph The graph on which to compute the shortest paths.
         */
        explicit Dijkstra(Graph &graph);

        /**
         * @brief Computes the shortest path from the source vertex to the sink vertex.
         * @param source The source vertex from which to start the shortest path computation.
         * @param sink The sink vertex to which to compute the shortest path. If not provided, computes shortest paths to all vertices.
         */
        void compute(const Vertex &source, const Vertex &sink = Vertex());

        /**
         * @brief Computes the shortest paths from multiple source vertices to the sink vertex.
         * @param sources The source vertices from which to start the shortest path computation.
         * @param sink The sink vertex to which to compute the shortest paths. If not provided, computes shortest paths to all vertices.
         */
        void compute(const std::vector<Vertex> &sources, const Vertex &sink = Vertex());

        /**
         * @brief Sets custom edge weights for the shortest path computation.
         * @param weights The custom edge weights to use.
         */
        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        /**
         * @brief Clears any custom edge weights set for the shortest path computation.
         */
        void clear_custom_edge_weights();

        EdgeProperty<Real> edge_weights; /**< The edge weights used in the shortest path computation. */
        VertexProperty<Real> vertex_distances; /**< The distances from the source vertex to each vertex. */
        VertexProperty<Halfedge> vertex_predecessors;
        /**< The predecessor halfedge for each vertex in the shortest path tree. */
    private:
        /**
         * @brief Clears the internal state of the Dijkstra object.
         */
        void clear();

        Graph &graph; /**< The graph on which to compute the shortest paths. */
    };
}
#endif //GRAPHDIJKSTRA_H
