//
// Created by alex on 2/3/25.
//

#include "GraphUtils.h"

namespace Bcg {
    EdgeProperty<Vector<unsigned int, 2> > Edges(Graph &graph) {
        auto indices = graph.edge_property<Vector<unsigned int, 2> >("e:indices");
        for (const Edge &e: graph.edges) {
            indices[e] = {graph.get_vertex(graph.get_halfedge(e, 0)).idx(),
                          graph.get_vertex(graph.get_halfedge(e, 1)).idx()};
        }
        return indices;
    }

    // A helper constant representing infinity.
    constexpr Real INF = std::numeric_limits<Real>::infinity();

    // A type alias for the elements stored in the priority queue.
    // Each element is a pair of (distance, vertex).
    using PQElement = std::pair<Real, Vertex>;


    // Computes shortest paths from a single source.
    void Dijkstra::compute(const Vertex &source) {
        // Initialize the vertex properties.
        const size_t n = graph.n_vertices();

        // Distance from source to itself is 0.
        vertex_distances[source] = 0;

        // Min-heap priority queue ordered by distance.
        std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> queue;
        queue.push({0, source});

        while (!queue.empty()) {
            auto [current_distance, current] = queue.top();
            queue.pop();

            // If we already found a better path, skip.
            if (current_distance > vertex_distances[current])
                continue;

            // Iterate over all neighbor vertices of 'current'.
            for (const Vertex &neighbor: graph.get_vertices(current)) {
                // Retrieve the halfedge connecting current and neighbor.
                Halfedge h = graph.find_halfedge(current, neighbor);
                Edge e = graph.get_edge(h);

                // Retrieve the weight for this edge.
                Real weight = 1.0;
                // Check if the custom edge_weights property contains a value for this edge.
                // (Assumes that if an edge is not assigned a weight in edge_weights, it defaults to 1.0.)
                if (e.idx() < graph.edges.size()) {
                    weight = edge_weights[e];
                }

                // Dijkstra's algorithm does not support negative weights.
                if (weight < 0)
                    continue;

                // Relaxation step.
                Real new_distance = vertex_distances[current] + weight;
                if (new_distance < vertex_distances[neighbor]) {
                    vertex_distances[neighbor] = new_distance;
                    vertex_predecessors[neighbor] = current;
                    queue.push({new_distance, neighbor});
                }
            }
        }
    }

    // Computes shortest paths from multiple sources.
    void Dijkstra::compute(const std::vector<Vertex> &sources) {
        // Initialize the vertex properties.
        const size_t n = graph.n_vertices();

        std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> queue;
        // Set the distance for each source to 0.
        for (const Vertex &s: sources) {
            vertex_distances[s] = 0;
            queue.push({0, s});
        }

        while (!queue.empty()) {
            auto [current_distance, current] = queue.top();
            queue.pop();

            if (current_distance > vertex_distances[current])
                continue;

            for (const Vertex &neighbor: graph.get_vertices(current)) {
                Halfedge h = graph.find_halfedge(current, neighbor);
                Edge e = graph.get_edge(h);

                Real weight = 1.0;
                if (e.idx() < graph.edges.size()) {
                    weight = edge_weights[e];
                }
                if (weight < 0)
                    continue;

                Real new_distance = vertex_distances[current] + weight;
                if (new_distance < vertex_distances[neighbor]) {
                    vertex_distances[neighbor] = new_distance;
                    vertex_predecessors[neighbor] = current;
                    queue.push({new_distance, neighbor});
                }
            }
        }
    }
}