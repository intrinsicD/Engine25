//
// Created by alex on 2/11/25.
//

#ifndef GRAPHBELLMANFORD_H
#define GRAPHBELLMANFORD_H

#include "Graph.h"

namespace Bcg {
    /**
     * @brief BellmanFord: Computes shortest paths from a source (or multiple sources)
     * for graphs with possibly negative edge weights.
     */
    class BellmanFord {
    public:
        /**
         * @brief Constructs a BellmanFord object for the given graph.
         * @param graph The graph on which to compute the shortest paths.
         */
        explicit BellmanFord(Graph &graph);

        /**
         * @brief Computes the shortest path from the source vertex.
         * @param source The source vertex from which to start the shortest path computation.
         * @return True if the computation was successful, false if a negative cycle was detected.
         */
        bool compute(const Vertex &source);

        /**
         * @brief Computes the shortest paths from multiple source vertices.
         * @param sources The source vertices from which to start the shortest path computation.
         * @return True if the computation was successful, false if a negative cycle was detected.
         */
        bool compute(const std::vector<Vertex> &sources);

        /**
         * @brief Checks if a negative cycle exists in the graph.
         * @return True if a negative cycle is found, false otherwise.
         */
        bool has_negative_cycle() const;

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
        bool negative_cycle_found; /**< Indicates if a negative cycle was found during the computation. */

    private:
        /**
         * @brief Clears the internal state of the BellmanFord object.
         */
        void clear();

        Graph &graph; /**< The graph on which to compute the shortest paths. */
    };
}

#endif //GRAPHBELLMANFORD_H
