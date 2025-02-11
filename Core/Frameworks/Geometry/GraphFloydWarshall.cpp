//
// Created by alex on 2/11/25.
//

#include "GraphFloydWarshall.h"
#include "GraphUtils.h"

namespace Bcg {
    FloydWarshall::FloydWarshall(Graph &graph) : graph(graph) {

    }

    void FloydWarshall::compute() {
        clear(); // Allocates and initializes the matrices.
        long n = graph.n_vertices();

        // Set the distance from each vertex to itself as 0.
        vertex_vertex_distances.diagonal().setConstant(0);

        // Initialize distances based on direct edges.
        for (long i = 0; i < n; i++) {
            Vertex v(i);
            for (const auto &h: graph.get_halfedges(v)) {
                Vertex u = graph.get_vertex(h);
                long j = u.idx();
                Real weight = edge_weights[graph.get_edge(h)];
                // Update if this direct edge is better.
                if (weight < vertex_vertex_distances(i, j)) {
                    vertex_vertex_distances(i, j) = weight;
                    // Store the predecessor as the halfedge from u back to v.
                    vertex_vertex_predecessors(i, j) = graph.get_opposite(h);
                }
            }
        }

        // Run the Floyd–Warshall triple loop.
        for (long k = 0; k < n; k++) {
            for (long i = 0; i < n; i++) {
                // Only consider paths if the intermediate vertex is reachable.
                if (vertex_vertex_distances(i, k) == std::numeric_limits<Real>::max())
                    continue;
                for (long j = 0; j < n; j++) {
                    // Check that k -> j is reachable.
                    if (vertex_vertex_distances(k, j) == std::numeric_limits<Real>::max())
                        continue;
                    Real new_distance = vertex_vertex_distances(i, k) + vertex_vertex_distances(k, j);
                    if (new_distance < vertex_vertex_distances(i, j)) {
                        vertex_vertex_distances(i, j) = new_distance;
                        // The new predecessor for path i->j is the predecessor on the path from k to j.
                        vertex_vertex_predecessors(i, j) = vertex_vertex_predecessors(k, j);
                    }
                }
            }
        }
    }

    void FloydWarshall::set_custom_edge_weights(const EdgeProperty<Real> &weights) {
        edge_weights = weights;
    }

    void FloydWarshall::clear_custom_edge_weights() {
        // Revert to the default edge weights.
        edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
    }

    void FloydWarshall::clear() {
        if (!edge_weights) {
            edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
        }

        long n = graph.n_vertices();
        // Resize matrices to n x n.
        vertex_vertex_distances.resize(n, n);
        vertex_vertex_predecessors.resize(n, n);

        // Initialize all distances to infinity.
        vertex_vertex_distances.setConstant(std::numeric_limits<Real>::max());
        // Initialize all predecessors to an invalid halfedge.
        vertex_vertex_predecessors.setConstant(Halfedge());
    }
}
