//
// Created by alex on 2/11/25.
//

#include "GraphConnectedComponents.h"

namespace Bcg{

    ConnectedComponents::ConnectedComponents(Graph &graph) : graph(graph){

    }

    void ConnectedComponents::compute() {
        clear(); // Reset all component IDs to -1.
        int comp_id = 0;

        // Iterate over all vertices by index.
        for (const auto &v : graph.vertices) {
            // If the vertex hasn't been assigned a component id yet.
            if (component_ids[v] == -1) {
                // Traverse the connected component using BFS.
                for (const Vertex &u : graph.bfs(v)) {
                    component_ids[u] = comp_id;
                }
                comp_id++;
            }
        }
    }

    void ConnectedComponents::clear() {
        if (!component_ids) {
            component_ids = graph.vertex_property<int>("v:component_ids", -1);
        }else {
            std::ranges::fill(component_ids.vector(), -1);
        }
    }
}