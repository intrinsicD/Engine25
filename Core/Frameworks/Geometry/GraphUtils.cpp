//
// Created by alex on 2/3/25.
//

#include "GraphUtils.h"

namespace Bcg {
    EdgeProperty<Vector<unsigned int, 2> > Edges(Graph &graph) {
        auto indices = graph.edge_property<Vector<unsigned int, 2> >("e:indices");
        for (const Edge &e: graph.edges) {
            indices[e] = {graph.get_vertex(graph.get_halfedge(e, 0)).idx(), graph.get_vertex(graph.get_halfedge(e, 1)).idx()};
        }
        return indices;
    }
}