//
// Created by alex on 2/11/25.
//

#include "GraphKruskal.h"
#include "GraphUtils.h"
#include <numeric>

namespace Bcg {
    Kruskal::Kruskal(Graph &graph) : graph(graph) {
    }

    // A simple union–find (disjoint-set) structure.
    struct UnionFind {
        std::vector<int> parent;

        UnionFind(int n) : parent(n) {
            std::iota(parent.begin(), parent.end(), 0);
        }

        int find(int a) {
            if (parent[a] != a)
                parent[a] = find(parent[a]);
            return parent[a];
        }

        bool unionSets(int a, int b) {
            int rootA = find(a);
            int rootB = find(b);
            if (rootA == rootB) return false;
            parent[rootB] = rootA;
            return true;
        }
    };

    void Kruskal::compute(const Vertex &source) {
        clear(); // Reset predecessor property and ensure edge_weights is set.

        int nV = static_cast<int>(graph.n_vertices());
        int nE = static_cast<int>(graph.n_edges());

        // Collect all edges.
        std::vector<Edge> edges;
        edges.reserve(nE);
        for (int i = 0; i < nE; i++) {
            edges.push_back(Edge(i));
        }

        // Sort edges by increasing weight.
        std::sort(edges.begin(), edges.end(), [&](const Edge &a, const Edge &b) {
            return edge_weights[a] < edge_weights[b];
        });

        // Initialize union-find structure.
        UnionFind uf(nV);
        // in_mst will mark which edges are part of the MST.
        std::vector<bool> in_mst(nE, false);

        // Process edges in sorted order.
        for (const Edge &e: edges) {
            // For each edge, get its two endpoints.
            Halfedge h0 = graph.get_halfedge(e, 0);
            Halfedge h1 = graph.get_halfedge(e, 1);
            Vertex v0 = graph.get_vertex(h0);
            Vertex v1 = graph.get_vertex(h1);
            // If v0 and v1 are in different components, add the edge.
            if (uf.unionSets(v0.idx(), v1.idx())) {
                in_mst[e.idx()] = true;
            }
        }

        // Now, to produce a predecessor mapping (i.e. an MST rooted at the given source),
        // we perform a BFS on the MST.
        std::vector<bool> visited(nV, false);
        std::queue<Vertex> q;
        visited[source.idx()] = true;
        q.push(source);

        while (!q.empty()) {
            Vertex cur = q.front();
            q.pop();

            // Explore all outgoing halfedges from cur.
            for (const auto &h: graph.get_halfedges(cur)) {
                Edge e = graph.get_edge(h);
                // Only follow edges that were included in the MST.
                if (!in_mst[e.idx()]) continue;
                Vertex nbr = graph.get_vertex(h);
                if (!visited[nbr.idx()]) {
                    visited[nbr.idx()] = true;
                    // We want the predecessor of nbr to be cur.
                    // Since h goes from cur to nbr, the opposite halfedge goes from nbr to cur.
                    vertex_predecessors[nbr] = graph.get_opposite(h);
                    q.push(nbr);
                }
            }
        }
    }

    void Kruskal::set_custom_edge_weights(const EdgeProperty<Real> &weights) {
        edge_weights = weights;
    }

    void Kruskal::clear_custom_edge_weights() {
        edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
    }

    void Kruskal::clear() {
        if (!edge_weights) {
            edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
        }

        // Initialize or reset the predecessor property.
        if (!vertex_predecessors) {
            vertex_predecessors = graph.vertex_property<Halfedge>("v:prim:predecessors", Halfedge());
        } else {
            std::ranges::fill(vertex_predecessors.vector(), Halfedge());
        }
    }
}
