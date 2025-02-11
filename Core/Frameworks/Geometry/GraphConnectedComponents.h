//
// Created by alex on 2/11/25.
//

#ifndef GRAPHCONNECTEDCOMPONENTS_H
#define GRAPHCONNECTEDCOMPONENTS_H

#include "Graph.h"

namespace Bcg{
    class ConnectedComponents {
    public:
        explicit ConnectedComponents(Graph &graph);

        void compute();

        VertexProperty<int> component_ids;

    private:
        void clear();

        Graph &graph;
    };
}

#endif //GRAPHCONNECTEDCOMPONENTS_H
