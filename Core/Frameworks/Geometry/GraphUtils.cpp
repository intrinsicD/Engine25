//
// Created by alex on 2/3/25.
//

#include "GraphUtils.h"
#include <queue>

namespace Bcg {
    EdgeProperty<Vector<unsigned int, 2> > Edges(Graph &graph) {
        auto indices = graph.edge_property<Vector<unsigned int, 2> >("e:indices");
        for (const Edge &e: graph.edges) {
            indices[e] = {graph.get_vertex(graph.get_halfedge(e, 0)).idx(),
                          graph.get_vertex(graph.get_halfedge(e, 1)).idx()};
        }
        return indices;
    }

    EdgeProperty<Real> EdgeLengths(Graph &graph, const VertexProperty<Vector<Real, 3>> &positions) {
        auto lengths = graph.edge_property<Real>("e:length");
        for (const Edge &e: graph.edges) {
            lengths[e] = Length(graph, e, positions);
        }
        return lengths;
    }

    Dijkstra::Dijkstra(Bcg::Graph &graph) : graph(graph) {
        edge_weights = graph.get_edge_property<Real>("e:length");
        vertex_distances = graph.vertex_property<Real>("v:dijkstra:distances");
        vertex_predecessors = graph.vertex_property<Halfedge>("v:dijkstra:predecessors");
    }

    struct PQItem{
        Vertex v;
        Real distance;
    };

    struct PQCompare{
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
            for (const auto &h : graph.get_halfedges(v_current)) {
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
        for (const Vertex &source : sources) {
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

            for (const auto &h : graph.get_halfedges(v_current)) {
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
        edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3>>("v:position"));
    }

    std::vector<Halfedge> Dijkstra::backtrace_sink_to_source(const Vertex &sink){
        std::vector<Halfedge> path;

        // If the sink is unreachable (i.e., its distance remains infinity),
        // return an empty path.
        if (vertex_distances[sink] == std::numeric_limits<Real>::max())
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
        std::reverse(path.begin(), path.end());
        return path;
    }

    void Dijkstra::clear() {
        if (!edge_weights) {
            edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3>>("v:position"));
        }

        if (!vertex_distances) {
            vertex_distances = graph.vertex_property<Real>("v:dijkstra:distances", std::numeric_limits<Real>::max());
        } else {
            std::fill(vertex_distances.vector().begin(), vertex_distances.vector().end(),
                      std::numeric_limits<Real>::max());
        }

        if (!vertex_predecessors) {
            vertex_predecessors = graph.vertex_property<Halfedge>("v:dijkstra:predecessors");
        } else {
            std::fill(vertex_predecessors.vector().begin(), vertex_predecessors.vector().end(), Halfedge());
        }
    }

}