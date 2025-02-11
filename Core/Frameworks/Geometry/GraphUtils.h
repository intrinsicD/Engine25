//
// Created by alex on 2/3/25.
//

#ifndef GRAPHUTILS_H
#define GRAPHUTILS_H

#include "Graph.h"

namespace Bcg {
    EdgeProperty<Vector<unsigned int, 2> > Edges(Graph &graph);

    template<typename T, int N>
    Vector<T, N> EdgeVector(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) {
        return pos[v1] - pos[v0];
    }

    template<typename T, int N>
    Vector<T, N> EdgeVector(const Graph &graph, const Edge &e, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(graph.get_vertex(e, 0), graph.get_vertex(e, 1), pos);
    }

    template<typename T, int N>
    Vector<T, N> EdgeVector(const Graph &graph, const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(graph.get_edge(h), pos);
    }

    template<typename T, int N>
    Real Length(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(v0, v1, pos).norm();
    }

    template<typename T, int N>
    Real Length(const Edge &e, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(e, pos).norm();
    }

    template<typename T, int N>
    Real Length(const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(h, pos).norm();
    }

    template<typename T, int N>
    Vector<T, N> Center(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) {
        return (pos[v0] + pos[v1]) / 2;
    }

    template<typename T, int N>
    Vector<T, N> Center(const Graph &graph, const Edge &e, const VertexProperty<Vector<T, N> > &pos) {
        return Center(graph.get_vertex(e, 0), graph.get_vertex(e, 1), pos);
    }

    template<typename T, int N>
    Vector<T, N> Center(const Graph &graph, const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) {
        return Center(graph.get_edge(h), pos);
    }

    //------------------------------------------------------------------------------
    // Shortest Path Algorithms
    //------------------------------------------------------------------------------

    // Dijkstra: Computes shortest paths from a source (or multiple sources)
    // for graphs with non-negative edge weights.
    class Dijkstra {
    public:
        explicit Dijkstra(Graph &graph) : graph(graph) {
            edge_weights = graph.edge_property<Real>("e:weights");
            vertex_distances = graph.vertex_property<Real>("v:dijkstra:distances");
            vertex_predecessors = graph.vertex_property<Vertex>("v:dijkstra:predecessors");
        }

        void compute(const Vertex &source);

        void compute(const std::vector<Vertex> &sources);

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        EdgeProperty<Real> edge_weights;
        VertexProperty<Real> vertex_distances;
        VertexProperty<Vertex> vertex_predecessors;
    private:
        Graph &graph;
    };

    // Bellman-Ford: Computes shortest paths even when negative edge weights exist.
    // Returns false if a negative cycle is detected.
    class BellmanFord {
    public:
        explicit BellmanFord(Graph &graph) : graph(graph), negative_cycle_found(false) {}

        // distances and predecessors are passed by reference to be filled.
        // Returns true if computation succeeded, false if a negative cycle was found.
        bool compute(const Vertex &source,
                     VertexProperty<Real> &distances,
                     VertexProperty<Vertex> &predecessors);

        bool has_negative_cycle() const;

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        EdgeProperty<Real> edge_weights;
        bool negative_cycle_found;
    private:
        Graph &graph;
    };

    // A*: An informed search algorithm that uses a heuristic function.
    class AStar {
    public:
        explicit AStar(Graph &graph) : graph(graph) {}

        // The heuristic function estimates the cost from a given vertex to the target.
        // Returns the computed vertex distances.
        void compute(const Vertex &source, const Vertex &target, std::function<Real(const Vertex &)> heuristic);

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        EdgeProperty<Real> edge_weights;
        VertexProperty<Real> vertex_distances;
    private:
        Graph &graph;
    };

    // Floyd-Warshall: Computes all-pairs shortest paths.
    // Returns a 2D vector where result[i][j] is the distance from vertex i to vertex j.
    class FloydWarshall {
    public:
        explicit FloydWarshall(Graph &graph) : graph(graph) {}

        void compute();

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        EdgeProperty<Real> edge_weights;
        Matrix<Real, -1, -1> vertex_vertex_distances;
    private:
        Graph &graph;
    };

    //------------------------------------------------------------------------------
    // Spanning Tree Algorithms
    //------------------------------------------------------------------------------

    // Prim: Computes a minimum spanning tree (MST) starting from a given vertex.
    class Prim {
    public:
        explicit Prim(Graph &graph) : graph(graph) {}

        // Returns a vector of edges that form the MST.
        void compute(const Vertex &start);

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        EdgeProperty<Real> edge_weights;
        std::vector<Edge> mst_edges;
    private:
        Graph &graph;
    };

    // Kruskal: Computes a minimum spanning tree by processing edges in sorted order.
    class Kruskal {
    public:
        explicit Kruskal(Graph &graph) : graph(graph) {}

        // Returns a vector of edges that form the MST.
        void compute();

        void set_custom_edge_weights(const EdgeProperty<Real> &weights);

        EdgeProperty<Real> edge_weights;
        std::vector<Edge> mst_edges;
    private:
        Graph &graph;
    };

    //------------------------------------------------------------------------------
    // Connectivity and Component Analysis
    //------------------------------------------------------------------------------

    // ConnectedComponents: Computes connected components in an undirected graph.
    // Returns a vertex property mapping each vertex to its component id.
    class ConnectedComponents {
    public:
        explicit ConnectedComponents(Graph &graph) : graph(graph) {}

        void compute();

        VertexProperty<int> connected_components;
    private:
        Graph &graph;
    };

    // StronglyConnectedComponents: Computes strongly connected components in a directed graph.
    // Returns a vertex property mapping each vertex to its component id.
    class StronglyConnectedComponents {
    public:
        explicit StronglyConnectedComponents(Graph &graph) : graph(graph) {}

        void compute();

        VertexProperty<int> strongly_connected_components;
    private:
        Graph &graph;
    };

    //------------------------------------------------------------------------------
    // Cycle Detection and Topological Ordering
    //------------------------------------------------------------------------------

    // CycleDetection: Provides functionality to detect cycles in the graph.
    class CycleDetection {
    public:
        explicit CycleDetection(Graph &graph) : graph(graph) {}

        // Returns true if the graph contains a cycle.
        bool has_cycle();

        // If a cycle exists, returns one cycle as a vector of vertices.
        void compute_cycle();

        std::vector<Vertex> cycle;
    private:
        Graph &graph;
    };

    // TopologicalSort: Computes a topological ordering for a DAG.
    // Returns an empty vector if the graph is not acyclic.
    class TopologicalSort {
    public:
        explicit TopologicalSort(Graph &graph) : graph(graph) {}

        void compute();

        std::vector<Vertex> topological_order;
    private:
        Graph &graph;
    };

    //------------------------------------------------------------------------------
    // Network Flow Algorithms
    //------------------------------------------------------------------------------

    // EdmondsKarp: Implements the Edmonds-Karp algorithm for maximum flow.
    class EdmondsKarp {
    public:
        explicit EdmondsKarp(Graph &graph) : graph(graph) {}

        // Computes the maximum flow from source to sink.
        void compute(const Vertex &source, const Vertex &sink);

        // Optionally returns the residual graph after flow computation.
        void get_residual_graph();

        void set_edge_capacities(const EdgeProperty<Real> &capacities);

        EdgeProperty<Real> capacities;
        EdgeProperty<Real> flows;
        Real max_flow;
        Graph residual_graph;
    private:
        Graph &graph;
    };

    // Dinic: Implements Dinic's algorithm for maximum flow.
    class Dinic {
    public:
        explicit Dinic(Graph &graph) : graph(graph) {}

        // Computes the maximum flow from source to sink.
        void compute(const Vertex &source, const Vertex &sink);

        // Optionally returns the residual graph after flow computation.
        void get_residual_graph();

        void set_edge_capacities(const EdgeProperty<Real> &capacities);

        EdgeProperty<Real> capacities;
        EdgeProperty<Real> flows;
        Real max_flow;
        Graph residual_graph;
    private:
        Graph &graph;
    };

    //------------------------------------------------------------------------------
    // Graph Matching and Covering
    //------------------------------------------------------------------------------

    // BipartiteMatching: Computes a maximum matching for a bipartite graph.
    // Returns a vertex property mapping each vertex to its match (or an invalid vertex if unmatched).
    class BipartiteMatching {
    public:
        explicit BipartiteMatching(Graph &graph) : graph(graph) {}

        void compute();

        VertexProperty<Vertex> matching;
    private:
        Graph &graph;
    };

    // BipartiteVertexCover: Computes a minimum vertex cover for a bipartite graph.
    // Returns a vector of vertices that form the vertex cover.
    class BipartiteVertexCover {
    public:
        explicit BipartiteVertexCover(Graph &graph) : graph(graph) {}

        void compute();

        std::vector<Vertex> vertex_cover;
    private:
        Graph &graph;
    };
}

#endif //GRAPHUTILS_H
