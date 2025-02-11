//
// Created by alex on 2/11/25.
//

#include "GraphPrim.h"
#include "GraphUtils.h"

namespace Bcg {
    struct PrimPQItem {
        Vertex v;
        Real key;
    };

    struct PrimPQCompare {
        bool operator()(const PrimPQItem &a, const PrimPQItem &b) const {
            return a.key > b.key; // min-heap: lower key has higher priority
        }
    };

    Prim::Prim(Graph &graph) : graph(graph) {

    }

    void Prim::compute(const Vertex &source) {
        clear();

        const int n = static_cast<int>(graph.n_vertices());
        // in_tree[i] indicates if vertex with index i is already included in the MST.
        std::vector<bool> in_tree(n, false);
        // key[i] is the minimum weight to connect vertex i to the MST.
        std::vector<Real> key(n, std::numeric_limits<Real>::max());

        // Start with the source.
        key[source.idx()] = 0;

        std::priority_queue<PrimPQItem, std::vector<PrimPQItem>, PrimPQCompare> queue;
        queue.push({source, 0});

        while (!queue.empty()) {
            PrimPQItem current_item = queue.top();
            queue.pop();

            Vertex v = current_item.v;
            int vidx = v.idx();

            // Skip if already added to the MST.
            if (in_tree[vidx])
                continue;
            in_tree[vidx] = true;

            // Examine all outgoing halfedges of v.
            for (const auto &h: graph.get_halfedges(v)) {
                // h is a halfedge from v to its neighbor u.
                Vertex u = graph.get_vertex(h);
                int uidx = u.idx();
                // Only consider vertices not yet in the MST.
                if (!in_tree[uidx]) {
                    Edge e = graph.get_edge(h);
                    Real weight = edge_weights[e];
                    // If the weight of this edge is less than the current key for u,
                    // update key and store the connecting edge.
                    if (weight < key[uidx]) {
                        key[uidx] = weight;
                        // Store the predecessor as the halfedge from u back to v.
                        // (Since h goes from v to u, its opposite goes from u to v.)
                        vertex_predecessors[u] = graph.get_opposite(h);
                        queue.push({u, key[uidx]});
                    }
                }
            }
        }
    }

    void Prim::set_custom_edge_weights(const EdgeProperty<Real> &weights) {
        edge_weights = weights;
    }

    void Prim::clear_custom_edge_weights() {
        edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
    }

    void Prim::clear() {
        if (!edge_weights) {
            edge_weights = EdgeLengths(graph, graph.get_vertex_property<Vector<Real, 3> >("v:position"));
        }

        // Initialize or reset the predecessor property.
        if (!vertex_predecessors) {
            vertex_predecessors = graph.vertex_property<Halfedge>("v:prim:predecessors");
        } else {
            std::ranges::fill(vertex_predecessors.vector(), Halfedge());
        }
    }
}
