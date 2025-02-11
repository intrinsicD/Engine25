//
// Created by alex on 2/11/25.
//

#ifndef GRAPHASTAR_H
#define GRAPHASTAR_H

#include "Graph.h"

namespace Bcg {
    /**
     * @brief AStar: An informed search algorithm that uses a heuristic function.
     */
    class AStar {
    public:
        /**
         * @brief Constructs an AStar object for the given graph.
         * @param graph The graph on which to perform the A* search.
         */
        explicit AStar(Graph &graph);

        /**
         * @brief Computes the shortest path from the source vertex to the target vertex.
         * @param source The source vertex from which to start the search.
         * @param target The target vertex to which to find the shortest path.
         */
        void compute(const Vertex &source, const Vertex &target);

        /**
         * @brief Sets the heuristic function for the A* search.
         * @param heuristic The heuristic function to use.
         */
        void set_heuristic(std::function<Real(const Vertex &)> heuristic);

        /**
         * @brief Clears the heuristic function set for the A* search.
         */
        void clear_heuristic();

        /**
         * @brief Sets custom edge weights for the A* search.
         * @param weights The custom edge weights to use.
         */
        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        /**
         * @brief Clears any custom edge weights set for the A* search.
         */
        void clear_custom_edge_weights();

        EdgeProperty<Real> edge_weights; /**< The edge weights used in the A* search. */
        VertexProperty<Real> vertex_distances; /**< The distances from the source vertex to each vertex. */
        VertexProperty<Halfedge> vertex_predecessors;
        /**< The predecessor halfedge for each vertex in the shortest path tree. */
        std::function<Real(const Vertex &)> heuristic; /**< The heuristic function used in the A* search. */

    private:
        /**
         * @brief Clears the internal state of the AStar object.
         */
        void clear();

        Graph &graph; /**< The graph on which to perform the A* search. */
    };
}

#endif //GRAPHASTAR_H
