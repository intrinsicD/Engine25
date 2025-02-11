//
// Created by alex on 2/11/25.
//

#ifndef GRAPHKRUSKAL_H
#define GRAPHKRUSKAL_H

#include "Graph.h"

namespace Bcg {
    /**
     * @brief Kruskal: Computes a minimum spanning tree by processing edges in sorted order.
     */
    class Kruskal {
    public:
        /**
         * @brief Constructs a Kruskal object for the given graph.
         * @param graph The graph on which to compute the MST.
         */
        explicit Kruskal(Graph &graph);

        /**
         * @brief Computes the MST starting from the given source vertex.
         * @param source The source vertex from which to start the MST computation.
         */
        void compute(const Vertex &source);

        /**
         * @brief Sets custom edge weights for the MST computation.
         * @param weights The custom edge weights to use.
         */
        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        /**
         * @brief Clears any custom edge weights set for the MST computation.
         */
        void clear_custom_edge_weights();

        EdgeProperty<Real> edge_weights; /**< The edge weights used in the MST computation. */
        VertexProperty<Halfedge> vertex_predecessors;
        /**< Predecessor halfedge for each vertex in the MST to the source. */

    private:
        /**
         * @brief Clears the internal state of the Kruskal object.
         */
        void clear();

        Graph &graph; /**< The graph on which to compute the MST. */
    };
}

#endif //GRAPHKRUSKAL_H
