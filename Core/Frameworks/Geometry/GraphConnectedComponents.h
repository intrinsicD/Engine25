//
// Created by alex on 2/11/25.
//

#ifndef GRAPHCONNECTEDCOMPONENTS_H
#define GRAPHCONNECTEDCOMPONENTS_H

#include "Graph.h"

namespace Bcg {
    /**
     * @brief ConnectedComponents: Computes the connected components of a graph.
     */
    class ConnectedComponents {
    public:
        /**
         * @brief Constructs a ConnectedComponents object for the given graph.
         * @param graph The graph on which to compute the connected components.
         */
        explicit ConnectedComponents(Graph &graph);

        /**
         * @brief Computes the connected components of the graph.
         */
        void compute();

        VertexProperty<int> component_ids; /**< The component IDs for each vertex in the graph. */

    private:
        /**
         * @brief Clears the internal state of the ConnectedComponents object.
         */
        void clear();

        Graph &graph; /**< The graph on which to compute the connected components. */
    };
}

#endif //GRAPHCONNECTEDCOMPONENTS_H
