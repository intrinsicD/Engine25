//
// Created by alex on 2/11/25.
//

#include "GraphDijkstra.h"
#include "GraphUtils.h"

namespace Bcg {
    Dijkstra::Dijkstra(Graph &graph) : graph(graph) {

    }

    struct PQItem {
        Vertex v;
        Real distance;
    };

    struct PQCompare {
        bool operator()(const PQItem &a, const PQItem &b) const {
            return a.distance > b.distance;
        }
    };

    void Dijkstra::compute(const Vertex &source, const Vertex &sink) {
        clear();

        vertex_distances[source] = 0;

        std::priority_queue<PQItem, std::vector<PQItem>, PQCompare> queue;
        queue.push({source, 0});

        while (!queue.empty()) {
            PQItem current_item = queue.top();
            queue.pop();

            Vertex v_current = current_item.v;
            Real current_distance = current_item.distance;

            // If this is an outdated entry, skip.
            if (current_distance > vertex_distances[v_current])
                continue;

            // Optional early stopping: if we reached the sink, we can stop.
            if (sink.is_valid() && v_current == sink)
                break;

            // Iterate over all neighbors of the current vertex.
            for (const auto &h: graph.get_halfedges(v_current)) {
                // Retrieve the halfedge connecting current to neighbor.

                Edge e = graph.get_edge(h);
                Real weight = edge_weights[e];

                // Optionally, you may want to handle negative weights here.
                if (weight < 0)
                    continue;

                Vertex v_neighbor = graph.get_vertex(h);
                Real new_distance = vertex_distances[v_current] + weight;
                // If a shorter path is found, update the distance and predecessor.
                if (new_distance < vertex_distances[v_neighbor]) {
                    vertex_distances[v_neighbor] = new_distance;
                    // Store the predecessor as the opposite of h,
                    // which is the halfedge from v_neighbor back to v_current.
                    vertex_predecessors[v_neighbor] = graph.get_opposite(h);
                    queue.push({v_neighbor, new_distance});
                }
            }
        }
    }

    void Dijkstra::compute(const std::vector<Vertex> &sources, const Vertex &sink) {
        clear();

        std::priority_queue<PQItem, std::vector<PQItem>, PQCompare> queue;

        // Initialize the priority queue with all source vertices.
        for (const Vertex &source: sources) {
            vertex_distances[source] = 0;
            queue.push({source, 0});
        }

        while (!queue.empty()) {
            PQItem current_item = queue.top();
            queue.pop();

            Vertex v_current = current_item.v;
            Real current_distance = current_item.distance;

            // If this is an outdated entry, skip.
            if (current_distance > vertex_distances[v_current])
                continue;

            // Optional early stopping: if we reached the sink, we can stop.
            if (sink.is_valid() && v_current == sink)
                break;

            for (const auto &h: graph.get_halfedges(v_current)) {
                Edge e = graph.get_edge(h);
                Real weight = edge_weights[e];

                if (weight < 0)
                    continue;

                Vertex v_neighbor = graph.get_vertex(h);
                Real new_distance = vertex_distances[v_current] + weight;
                if (new_distance < vertex_distances[v_neighbor]) {
                    vertex_distances[v_neighbor] = new_distance;
                    // Store the predecessor as the opposite of h,
                    // which is the halfedge from v_neighbor back to v_current.
                    vertex_predecessors[v_neighbor] = graph.get_opposite(h);
                    queue.push({v_neighbor, new_distance});
                }
            }
        }
    }

    void Dijkstra::set_custom_edge_weights(const EdgeProperty<Real> &weights) {
        edge_weights = weights;
    }

    void Dijkstra::clear_custom_edge_weights() {
        edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
    }

    void Dijkstra::clear() {
        if (!edge_weights) {
            edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
        }

        if (!vertex_distances) {
            vertex_distances = graph.vertex_property<Real>("v:dijkstra:distances", std::numeric_limits<Real>::max());
        } else {
            std::ranges::fill(vertex_distances.vector(), std::numeric_limits<Real>::max());
        }

        if (!vertex_predecessors) {
            vertex_predecessors = graph.vertex_property<Halfedge>("v:dijkstra:predecessors");
        } else {
            std::ranges::fill(vertex_predecessors.vector(), Halfedge());
        }
    }
}
