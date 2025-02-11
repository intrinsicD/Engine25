//
// Created by alex on 2/11/25.
//

#ifndef GRAPHFLOYDWARSHALL_H
#define GRAPHFLOYDWARSHALL_H

#include "Graph.h"

namespace Bcg {
    /**
     * @brief Floyd-Warshall: Computes all-pairs shortest paths.
     */
    class FloydWarshall {
    public:
        /**
         * @brief Constructs a FloydWarshall object for the given graph.
         * @param graph The graph on which to compute the shortest paths.
         */
        explicit FloydWarshall(Graph &graph);

        /**
         * @brief Computes the all-pairs shortest paths.
         */
        void compute();

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
        Matrix<Real, -1, -1> vertex_vertex_distances; /**< Matrix of distances between vertices. */
        Matrix<Halfedge, -1, -1> vertex_vertex_predecessors;
        /**< Matrix of predecessor halfedges for each vertex pair. */

    private:
        /**
         * @brief Clears the internal state of the FloydWarshall object.
         */
        void clear();

        Graph &graph; /**< The graph on which to compute the shortest paths. */
    };
}

#endif //GRAPHFLOYDWARSHALL_H
