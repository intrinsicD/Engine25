//
// Created by alex on 2/3/25.
//

#include "GraphUtils.h"
#include <queue>

namespace Bcg {
    EdgeProperty<Vector<unsigned int, 2> > Edges(Graph &graph) {
        auto indices = graph.edge_property<Vector<unsigned int, 2> >("e:indices");
        for (const Edge &e: graph.edges) {
            indices[e] = {
                graph.get_vertex(graph.get_halfedge(e, 0)).idx(),
                graph.get_vertex(graph.get_halfedge(e, 1)).idx()
            };
        }
        return indices;
    }

    std::vector<Halfedge> BacktracePathSinkToSource(const Graph &graph, VertexProperty<Halfedge> vertex_predecessors,
                                                    const Vertex &sink, bool reverse) {
        std::vector<Halfedge> path;

        // If the sink is unreachable (i.e., its predecessor is invalid),
        // return an empty path.
        if (!vertex_predecessors[sink].is_valid())
            return path;

        Vertex current = sink;
        // Trace back from the sink to the source using the predecessor mapping.
        while (true) {
            Halfedge h_pred = vertex_predecessors[current];
            // If no valid predecessor exists, we have reached the source.
            if (!h_pred.is_valid())
                break;

            path.push_back(h_pred);
            current = graph.get_vertex(h_pred);
        }

        // The path is currently from sink to source. Reverse it to obtain a source-to-sink path.
        if (reverse) {
            std::ranges::reverse(path);
        }
        return path;
    }

    std::vector<Halfedge> BacktracePathSinkToSource(const Graph &graph,
                                                    const Matrix<Halfedge, -1, -1> &vertex_vertex_predecessors,
                                                    const Vertex &source, const Vertex &sink, bool reverse) {
        std::vector<Halfedge> path;

        // Check if there's any valid path from source to sink.
        if (!vertex_vertex_predecessors(source.idx(), sink.idx()).is_valid())
            return path;

        // Start at the sink and work backward.
        Vertex current = sink;
        while (current != source) {
            // Get the halfedge that was stored for the path from source to current.
            Halfedge h = vertex_vertex_predecessors(source.idx(), current.idx());
            if (!h.is_valid()) {
                // If at any point the predecessor is invalid, no valid path exists.
                path.clear();
                return path;
            }
            path.push_back(h);
            // Update current to be the predecessor vertex.
            current = graph.get_vertex(h);
        }

        // The path was built in reverse order (from sink to source),
        // so reverse it to get the path from source to sink.
        if (reverse) {
            std::reverse(path.begin(), path.end());
        }
        return path;
    }
}
