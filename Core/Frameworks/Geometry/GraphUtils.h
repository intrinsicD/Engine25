//
// Created by alex on 2/3/25.
//

#ifndef GRAPHUTILS_H
#define GRAPHUTILS_H

#include "Graph.h"

namespace Bcg {
    //------------------------------------------------------------------------------------------------------------------
    // Vertex Methods
    //------------------------------------------------------------------------------------------------------------------

    /**
     * @brief Computes the center of a vertex in the graph.
     *
     * This function calculates the centroid of the neighboring vertices of the given vertex
     * in the graph. The centroid is the arithmetic mean of the positions of the neighboring vertices.
     *
     * @tparam T The type of the coordinates (e.g., float, double).
     * @tparam N The number of dimensions.
     * @param graph The graph containing the vertices.
     * @param positions The vector of positions of the vertices.
     * @param v The vertex for which to compute the center.
     * @return The center point of the neighboring vertices.
     */
    template<typename T, int N>
    Vector<T, N> Center(const Graph &graph, const VertexProperty<Vector<Real, 3> > &positions, const Vertex &v) {
        Vector<double, N> center = Vector<double, N>::Zero();
        double count = 0.0;

        if (!graph.is_isolated(v)) {
            for (const auto vv: graph.get_vertices(v)) {
                center += positions[vv].template cast<double>();
                ++count;
            }
        }
        return (center / count).template cast<T>();
    }

    /**
     * @brief Computes the weighted center of a vertex in the graph.
     *
     * This function calculates the weighted centroid of the neighboring vertices of the given vertex
     * in the graph. The weights are inversely proportional to the distance between the vertices.
     *
     * @tparam T The type of the coordinates (e.g., float, double).
     * @tparam N The number of dimensions.
     * @param graph The graph containing the vertices.
     * @param positions The vector of positions of the vertices.
     * @param v The vertex for which to compute the weighted center.
     * @return The weighted center point of the neighboring vertices.
     */
    template<typename T, int N>
    Vector<T, N>
    WeightedCenter(const Graph &graph, const VertexProperty<Vector<Real, 3> > &positions, const Vertex &v) {
        Vector<double, N> center = Vector<double, N>::Zero();

        if (!graph.is_isolated(v)) {
            double sum_weights(0.0);
            for (const auto vv: graph.get_vertices(v)) {
                double weight = 1.0 / (positions[vv] - positions[v]).norm();
                center += weight * positions[vv].template cast<double>();
                sum_weights += weight;
            }
            center /= sum_weights;
        }
        return center.template cast<T>();
    }

    /**
     * @brief Computes the weighted center of a vertex in the graph using edge weights.
     *
     * This function calculates the weighted centroid of the neighboring vertices of the given vertex
     * in the graph. The weights are provided by the edge weights.
     *
     * @tparam T The type of the coordinates (e.g., float, double).
     * @tparam N The number of dimensions.
     * @param graph The graph containing the vertices.
     * @param positions The vector of positions of the vertices.
     * @param edge_weights The weights of the edges.
     * @param v The vertex for which to compute the weighted center.
     * @return The weighted center point of the neighboring vertices.
     */
    template<typename T, int N>
    Vector<T, N>
    WeightedCenter(const Graph &graph, const VertexProperty<Vector<Real, 3> > &positions, const EdgeProperty<Real> &edge_weights, const Vertex &v) {
        Vector<double, N> center = Vector<double, N>::Zero();

        if (!graph.is_isolated(v)) {
            double sum_weights(0.0);
            for (const auto h: graph.get_halfedges(v)) {
                double weight = edge_weights[graph.get_edge(h)];
                auto vv = graph.get_vertex(h);
                center += weight * positions[vv].template cast<double>();
                sum_weights += weight;
            }
            center /= sum_weights;
        }
        return center.template cast<T>();
    }

    //------------------------------------------------------------------------------
    // Edge Methods
    //------------------------------------------------------------------------------

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
     template<typename T, int N>
    EdgeProperty<Real> EdgeLengths(Graph &graph, const VertexProperty<Vector<T, N> > &positions) {
        auto lengths = graph.edge_property<Real>("e:length");
        for (const Edge &e: graph.edges) {
            auto v0 = graph.get_vertex(e, 0);
            auto v1 = graph.get_vertex(e, 1);

            lengths[e] = (positions[v1].template cast<double>() - positions[v0].template cast<double>()).norm();
        }
        return lengths;
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
    Vector<T, N> EdgeCenter( const VertexProperty<Vector<T, N> > &positions, const Vertex &v0, const Vertex &v1) {
        return (positions[v0].template cast<double>() + positions[v1].template cast<double>()) / 2;
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
    Vector<T, N> EdgeCenter(const Graph &graph, const VertexProperty<Vector<T, N> > &positions, const Edge &e) {
        return EdgeCenter(positions, graph.get_vertex(e, 0), graph.get_vertex(e, 1));
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
    Vector<T, N> EdgeCenter(const Graph &graph,  const VertexProperty<Vector<T, N> > &positions, const Halfedge &h) {
        return EdgeCenter(graph, positions, graph.get_edge(h));
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
