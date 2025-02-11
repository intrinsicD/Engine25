//
// Created by alex on 2/3/25.
//

#ifndef GRAPHUTILS_H
#define GRAPHUTILS_H

#include "Graph.h"

namespace Bcg {
    /**
     * @brief Retrieves the edges of the graph.
     * @param graph The graph from which to retrieve edges.
     * @return An EdgeProperty containing the edges of the graph.
     */
    EdgeProperty<Vector<unsigned int, 2> > Edges(Graph &graph);

    /**
     * @brief Computes the lengths of the edges in the graph.
     * @param graph The graph containing the edges.
     * @param positions The positions of the vertices in the graph.
     * @return An EdgeProperty containing the lengths of the edges.
     */
    EdgeProperty<Real> EdgeLengths(Graph &graph, const VertexProperty<Vector<Real, 3> > &positions);

    /**
     * @brief Computes the vector representation of an edge given two vertices.
     * @tparam T The type of the vector components.
     * @tparam N The dimension of the vector.
     * @param v0 The first vertex of the edge.
     * @param v1 The second vertex of the edge.
     * @param pos The positions of the vertices.
     * @return The vector representation of the edge.
     */
    template<typename T, int N>
    Vector<T, N> EdgeVector(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) {
        return pos[v1] - pos[v0];
    }

    /**
     * @brief Computes the vector representation of an edge given the graph and an edge.
     * @tparam T The type of the vector components.
     * @tparam N The dimension of the vector.
     * @param graph The graph containing the edge.
     * @param e The edge.
     * @param pos The positions of the vertices.
     * @return The vector representation of the edge.
     */
    template<typename T, int N>
    Vector<T, N> EdgeVector(const Graph &graph, const Edge &e, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(graph.get_vertex(e, 0), graph.get_vertex(e, 1), pos);
    }

    /**
     * @brief Computes the vector representation of an edge given the graph and a halfedge.
     * @tparam T The type of the vector components.
     * @tparam N The dimension of the vector.
     * @param graph The graph containing the halfedge.
     * @param h The halfedge.
     * @param pos The positions of the vertices.
     * @return The vector representation of the edge.
     */
    template<typename T, int N>
    Vector<T, N> EdgeVector(const Graph &graph, const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(graph.get_edge(h), pos);
    }

    /**
     * @brief Computes the length of an edge given two vertices.
     * @tparam T The type of the vector components.
     * @tparam N The dimension of the vector.
     * @param v0 The first vertex of the edge.
     * @param v1 The second vertex of the edge.
     * @param pos The positions of the vertices.
     * @return The length of the edge.
     */
    template<typename T, int N>
    Real Length(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(v0, v1, pos).norm();
    }

    /**
     * @brief Computes the length of an edge given the graph and an edge.
     * @tparam T The type of the vector components.
     * @tparam N The dimension of the vector.
     * @param graph The graph containing the edge.
     * @param e The edge.
     * @param pos The positions of the vertices.
     * @return The length of the edge.
     */
    template<typename T, int N>
    Real Length(const Graph &graph, const Edge &e, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(graph, e, pos).norm();
    }

    /**
     * @brief Computes the length of an edge given the graph and a halfedge.
     * @tparam T The type of the vector components.
     * @tparam N The dimension of the vector.
     * @param graph The graph containing the halfedge.
     * @param h The halfedge.
     * @param pos The positions of the vertices.
     * @return The length of the edge.
     */
    template<typename T, int N>
    Real Length(const Graph &graph, const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) {
        return EdgeVector(graph, h, pos).norm();
    }

    /**
     * @brief Computes the center of an edge given two vertices.
     * @tparam T The type of the vector components.
     * @tparam N The dimension of the vector.
     * @param v0 The first vertex of the edge.
     * @param v1 The second vertex of the edge.
     * @param pos The positions of the vertices.
     * @return The center of the edge.
     */
    template<typename T, int N>
    Vector<T, N> Center(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) {
        return (pos[v0] + pos[v1]) / 2;
    }

    /**
     * @brief Computes the center of an edge given the graph and an edge.
     * @tparam T The type of the vector components.
     * @tparam N The dimension of the vector.
     * @param graph The graph containing the edge.
     * @param e The edge.
     * @param pos The positions of the vertices.
     * @return The center of the edge.
     */
    template<typename T, int N>
    Vector<T, N> Center(const Graph &graph, const Edge &e, const VertexProperty<Vector<T, N> > &pos) {
        return Center(graph.get_vertex(e, 0), graph.get_vertex(e, 1), pos);
    }

    /**
     * @brief Computes the center of an edge given the graph and a halfedge.
     * @tparam T The type of the vector components.
     * @tparam N The dimension of the vector.
     * @param graph The graph containing the halfedge.
     * @param h The halfedge.
     * @param pos The positions of the vertices.
     * @return The center of the edge.
     */
    template<typename T, int N>
    Vector<T, N> Center(const Graph &graph, const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) {
        return Center(graph.get_edge(h), pos);
    }

    //------------------------------------------------------------------------------
    // Shortest Path Algorithms
    //------------------------------------------------------------------------------

    std::vector<Halfedge> BacktracePathSinkToSource(const Graph &graph, VertexProperty<Halfedge> vertex_predecessors,
                                                    const Vertex &sink, bool reverse = false);

    std::vector<Halfedge> BacktracePathSinkToSource(const Graph &graph,
                                           const Matrix<Halfedge, -1, -1> &vertex_vertex_predecessors,
                                           const Vertex &source, const Vertex &sink, bool reverse = false);

    //------------------------------------------------------------------------------
    // Cycle Detection and Topological Ordering
    //------------------------------------------------------------------------------

    // CycleDetection: Provides functionality to detect cycles in the graph.
    class CycleDetection {
    public:
        explicit CycleDetection(Graph &graph) : graph(graph) {
        }

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
        explicit TopologicalSort(Graph &graph) : graph(graph) {
        }

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
        explicit EdmondsKarp(Graph &graph) : graph(graph) {
        }

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
        explicit Dinic(Graph &graph) : graph(graph) {
        }

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
        explicit BipartiteMatching(Graph &graph) : graph(graph) {
        }

        void compute();

        VertexProperty<Vertex> matching;

    private:
        Graph &graph;
    };

    // BipartiteVertexCover: Computes a minimum vertex cover for a bipartite graph.
    // Returns a vector of vertices that form the vertex cover.
    class BipartiteVertexCover {
    public:
        explicit BipartiteVertexCover(Graph &graph) : graph(graph) {
        }

        void compute();

        std::vector<Vertex> vertex_cover;

    private:
        Graph &graph;
    };
}

#endif //GRAPHUTILS_H
