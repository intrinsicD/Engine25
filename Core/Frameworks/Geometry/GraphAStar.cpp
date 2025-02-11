//
// Created by alex on 2/11/25.
//

#include "GraphAStar.h"

#include <utility>
#include "GraphUtils.h"

namespace Bcg {
    // Priority queue item for A* search.
    struct AStarPQItem {
        Vertex v;
        Real f; // f = g + h, where g is the cost so far and h is the heuristic estimate.
        Real g; // Cost from the source to this vertex.
    };

    // Comparator for the priority queue (min-heap based on f).
    struct AStarPQCompare {
        bool operator()(const AStarPQItem &a, const AStarPQItem &b) const {
            return a.f > b.f;
        }
    };

    AStar::AStar(Graph &graph) : graph(graph) {
        // Optionally, you could initialize the heuristic to a default lambda that returns 0.
        heuristic = [](const Vertex &) -> Real { return 0; };
    }

    void AStar::compute(const Vertex &source, const Vertex &target) {
        clear();

        // If no heuristic is set, use a default zero heuristic.
        if (!heuristic) {
            heuristic = [](const Vertex &) -> Real { return 0; };
        }

        // Initialize source.
        vertex_distances[source] = 0;
        Real f_source = vertex_distances[source] + heuristic(source);

        std::priority_queue<AStarPQItem, std::vector<AStarPQItem>, AStarPQCompare> queue;
        queue.push({source, f_source, 0});

        while (!queue.empty()) {
            AStarPQItem current_item = queue.top();
            queue.pop();

            Vertex v_current = current_item.v;
            Real g_current = current_item.g;

            // Skip outdated queue entries.
            if (g_current > vertex_distances[v_current])
                continue;

            // Early termination: if we've reached the target, stop.
            if (v_current == target)
                break;

            // Iterate over all outgoing halfedges.
            for (const auto &h: graph.get_halfedges(v_current)) {
                Edge e = graph.get_edge(h);
                Real weight = edge_weights[e];

                // Skip negative weights if encountered.
                if (weight < 0)
                    continue;

                Vertex v_neighbor = graph.get_vertex(h);
                Real new_g = vertex_distances[v_current] + weight;
                if (new_g < vertex_distances[v_neighbor]) {
                    vertex_distances[v_neighbor] = new_g;
                    // Store the predecessor as the opposite halfedge.
                    vertex_predecessors[v_neighbor] = graph.get_opposite(h);
                    Real new_f = new_g + heuristic(v_neighbor);
                    queue.push({v_neighbor, new_f, new_g});
                }
            }
        }
    }

    void AStar::set_heuristic(std::function<Real(const Vertex &)> h) {
        heuristic = std::move(h);
    }

    void AStar::clear_heuristic() {
        // Reset heuristic to a zero function.
        heuristic = [](const Vertex &) -> Real { return 0; };
    }

    void AStar::set_custom_edge_weights(const EdgeProperty<Real> &weights) {
        edge_weights = weights;
    }

    void AStar::clear_custom_edge_weights() {
        // Revert to the default edge weights computed from vertex positions.
        edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
    }

    void AStar::clear() {
        if (!edge_weights) {
            edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
        }

        // Clear or initialize vertex distances to infinity.
        if (!vertex_distances) {
            vertex_distances = graph.vertex_property<Real>("v:astar:distances", std::numeric_limits<Real>::max());
        } else {
            std::ranges::fill(vertex_distances.vector(), std::numeric_limits<Real>::max());
        }

        // Clear or initialize the predecessor property.
        if (!vertex_predecessors) {
            vertex_predecessors = graph.vertex_property<Halfedge>("v:astar:predecessors");
        } else {
            std::ranges::fill(vertex_predecessors.vector(), Halfedge());
        }
    }
}
