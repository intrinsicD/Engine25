//
// Created by alex on 2/11/25.
//

#include "GraphBellmanFord.h"
#include "GraphUtils.h"

namespace Bcg {
    BellmanFord::BellmanFord(Graph &graph): graph(graph), negative_cycle_found(false) {

    }

    bool BellmanFord::compute(const Vertex &source) {
        return compute({source});
    }

    bool BellmanFord::compute(const std::vector<Vertex> &sources) {
        clear();
        // Initialize all source vertices with a distance of zero.
        for (const Vertex &source: sources) {
            vertex_distances[source] = 0;
        }

        const size_t n = graph.n_vertices();

        for (size_t i = 0; i < n - 1; i++) {
            bool updated = false;
            for (const auto &v : graph.vertices) {
                // Only relax from reachable vertices.
                if (vertex_distances[v] < std::numeric_limits<Real>::max()) {
                    for (const auto &h: graph.get_halfedges(v)) {
                        Edge e = graph.get_edge(h);
                        Real weight = edge_weights[e];
                        Vertex u = graph.get_vertex(h);
                        if (vertex_distances[v] + weight < vertex_distances[u]) {
                            vertex_distances[u] = vertex_distances[v] + weight;
                            // Save the predecessor as the opposite halfedge,
                            // so that the target of the stored halfedge gives the predecessor.
                            vertex_predecessors[u] = graph.get_opposite(h);
                            updated = true;
                        }
                    }
                }
            }
            // Optional: if no update was made in an entire pass, we can stop early.
            if (!updated)
                break;
        }

        // Check for negative cycles.
        for (const auto &v : graph.vertices) {
            if (vertex_distances[v] < std::numeric_limits<Real>::max()) {
                for (const auto &h: graph.get_halfedges(v)) {
                    Edge e = graph.get_edge(h);
                    Real weight = edge_weights[e];
                    Vertex u = graph.get_vertex(h);
                    if (vertex_distances[v] + weight < vertex_distances[u]) {
                        negative_cycle_found = true;
                        return false; // Negative cycle detected.
                    }
                }
            }
        }
        return true;
    }

    bool BellmanFord::has_negative_cycle() const {
        return negative_cycle_found;
    }

    void BellmanFord::set_custom_edge_weights(const EdgeProperty<Real> &weights) {
        edge_weights = weights;
    }

    void BellmanFord::clear_custom_edge_weights() {
        edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
    }

    void BellmanFord::clear() {
        if (!edge_weights) {
            edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
        }

        if (!vertex_distances) {
            vertex_distances = graph.vertex_property<Real>("v:bellmanford:distances", std::numeric_limits<Real>::max());
        } else {
            std::ranges::fill(vertex_distances.vector(), std::numeric_limits<Real>::max());
        }

        if (!vertex_predecessors) {
            vertex_predecessors = graph.vertex_property<Halfedge>("v:bellmanford:predecessors");
        } else {
            std::ranges::fill(vertex_predecessors.vector(), Halfedge());
        }
        negative_cycle_found = false;
    }
}
