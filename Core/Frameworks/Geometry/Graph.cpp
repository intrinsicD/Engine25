//
// Created by alex on 08.01.25.
//

#include "Graph.h"
#include <queue>
#include <stack>

namespace Bcg {
    Graph::Graph() : PointCloud() {
        // link properties to containers
        positions = vertices.vertex_property<Vector<Real, 3> >("v:position");
        v_connectivity = vertices.vertex_property<VertexConnectivity>("v:connectivity");
        h_connectivity = halfedges.halfedge_property<HalfedgeConnectivity>("h:connectivity");
        v_deleted = vertices.vertex_property<bool>("v:deleted", false);
        h_deleted = halfedges.halfedge_property<bool>("h:deleted", false);
        e_deleted = edges.edge_property<bool>("e:deleted", false);
        e_direction = edges.edge_property<Halfedge>("e:direction");
    }

    Graph &Graph::operator=(const Graph &rhs) {
        if (this != &rhs) {
            // deep copy of property containers
            vertices = rhs.vertices;
            halfedges = rhs.halfedges;
            edges = rhs.edges;

            // link properties from copied containers
            positions = vertices.vertex_property<Vector<Real, 3> >("v:position");

            v_connectivity = vertices.vertex_property<VertexConnectivity>("v:connectivity");
            h_connectivity = halfedges.halfedge_property<HalfedgeConnectivity>("h:connectivity");

            v_deleted = vertices.vertex_property<bool>("v:deleted");
            h_deleted = halfedges.halfedge_property<bool>("h:deleted");
            e_deleted = edges.edge_property<bool>("e:deleted");

            // how many elements are deleted?
            vertices.num_deleted = rhs.vertices.num_deleted;
            halfedges.num_deleted = rhs.halfedges.num_deleted;
            edges.num_deleted = rhs.edges.num_deleted;
        }

        return *this;
    }

    Graph &Graph::assign(const Graph &rhs) {
        if (this != &rhs) {
            // clear properties
            clear();

            // resize (needed by property containers)
            vertices.resize(rhs.vertices.size());
            halfedges.resize(rhs.halfedges.size());
            edges.resize(rhs.edges.size());

            // copy properties from other mesh
            positions.vector() = rhs.positions.vector();

            v_connectivity.vector() = rhs.v_connectivity.vector();
            h_connectivity.vector() = rhs.h_connectivity.vector();

            v_deleted.vector() = rhs.v_deleted.vector();
            h_deleted.vector() = rhs.h_deleted.vector();
            e_deleted.vector() = rhs.e_deleted.vector();

            // how many elements are deleted?
            vertices.num_deleted = rhs.vertices.num_deleted;
            halfedges.num_deleted = rhs.halfedges.num_deleted;
            edges.num_deleted = rhs.edges.num_deleted;
        }

        return *this;
    }

    void Graph::clear() {
        vertices.clear();
        halfedges.clear();
        edges.clear();

        free_memory();

        v_deleted = vertices.vertex_property<bool>("v:deleted", false);
        h_deleted = halfedges.halfedge_property<bool>("h:deleted", false);
        e_deleted = edges.edge_property<bool>("e:deleted", false);

        positions = vertices.vertex_property<Vector<Real, 3> >("v:position", Vector<Real, 3>::Zero());
        v_connectivity = vertices.vertex_property<VertexConnectivity>("v:connectivity");
        h_connectivity = halfedges.halfedge_property<HalfedgeConnectivity>("h:connectivity");
    }

    void Graph::free_memory() {
        vertices.free_memory();
        halfedges.free_memory();
        edges.free_memory();
    }

    void Graph::reserve(size_t nvertices, size_t nedges) {
        PointCloud::reserve(nvertices);
        halfedges.reserve(2 * nedges);
        edges.reserve(nedges);
    }

    void Graph::garbage_collection() {
        if (!has_garbage()) return;

        size_t nV = vertices.size();
        size_t nE = edges.size();
        size_t nH = halfedges.size();

        auto vmap = vertices.vertex_property<Vertex>("v:garbage_collection");
        auto hmap = halfedges.halfedge_property<Halfedge>("h:garbage_collection");

        for (size_t i = 0; i < nV; ++i) {
            auto v = Vertex(i);
            vmap[v] = Vertex(v);
        }

        for (size_t i = 0; i < nH; ++i) {
            auto h = Halfedge(i);
            hmap[h] = Halfedge(h);
        }

        if (nV > 0) {
            size_t i0 = 0;
            size_t i1 = nV - 1;

            while (true) {
                // find first deleted and last un-deleted
                while (!v_deleted[Vertex(i0)] && i0 < i1) {
                    ++i0;
                }
                while (v_deleted[Vertex(i1)] && i0 < i1) {
                    --i1;
                }
                if (i0 >= i1) break;

                // swap
                vertices.swap(i0, i1);
            }

            // remember new size
            nV = v_deleted[Vertex(i0)] ? i0 : i0 + 1;
        }

        if (nE > 0) {
            size_t i0 = 0;
            size_t i1 = nE - 1;

            while (true) {
                // find first deleted and last un-deleted
                while (!e_deleted[Edge(i0)] && i0 < i1) {
                    ++i0;
                }
                while (e_deleted[Edge(i1)] && i0 < i1) {
                    --i1;
                }
                if (i0 >= i1) break;

                // swap
                edges.swap(i0, i1);
                halfedges.swap(2 * i0, 2 * i1);
                halfedges.swap(2 * i0 + 1, 2 * i1 + 1);
            }

            // remember new size
            nE = e_deleted[Edge(i0)] ? i0 : i0 + 1;
            nH = 2 * nE;
        }

        // update vertex connectivity
        for (size_t i = 0; i < nV; ++i) {
            auto v = Vertex(i);
            if (!is_isolated(v)) {
                set_halfedge(v, hmap[get_halfedge(v)]);
            }
        }

        for (size_t i = 0; i < nH; ++i) {
            auto h = Halfedge(i);
            set_vertex(h, vmap[get_vertex(h)]);
            auto next = get_next(h);
            if (halfedges.is_valid(next)) {
                set_next(h, hmap[next]);
            }
        }

        vertices.remove(vmap);
        halfedges.remove(hmap);

        vertices.resize(nV);
        halfedges.resize(nH);
        edges.resize(nE);
        free_memory();

        vertices.num_deleted = 0;
        halfedges.num_deleted = 0;
        edges.num_deleted = 0;

        assert(!has_garbage());
    }

    // Vertex Methods

    void Graph::delete_vertex(const Vertex &v) {
        if (v_deleted[v]) {
            return;
        }

        for (const Edge &e: get_edges(v)) {
            delete_edge(e);
        }

        PointCloud::mark_deleted(v);
    }

    size_t Graph::get_valence(const Vertex &v) const {
        auto vv = get_vertices(v);
        return std::distance(vv.begin(), vv.end());
    }

    size_t Graph::get_indegree(const Vertex &v) const {
        size_t count = 0;
        for (auto e: get_edges(v)) {
            auto h = e_direction[e];
            if (!h.is_valid() || get_vertex(h) == v) {
                ++count;
            }
        }
    }

    size_t Graph::get_outdegree(const Vertex &v) const {
        size_t count = 0;
        for (auto e: get_edges(v)) {
            auto h = e_direction[e];
            if (!h.is_valid() || get_vertex(h) != v) {
                ++count;
            }
        }
    }

    // Halfedge Methods

    void Graph::mark_deleted(const Halfedge &h) {
        if (h_deleted[h]) {
            return;
        }

        h_deleted[h] = true;
        ++halfedges.num_deleted;
    }

    Halfedge Graph::find_halfedge(const Vertex &v0, const Vertex &v1) const {
        assert(PointCloud::is_valid(v0) && PointCloud::is_valid(v1));

        Halfedge h = get_halfedge(v0);
        const Halfedge hh = h;

        if (h.is_valid()) {
            do {
                if (get_vertex(h) == v1)
                    return h;
                h = rotate_cw(h);
            } while (h != hh);
        }

        return {};
    }

    // Edge Methods


    Halfedge Graph::new_edge(const Vertex &v0, const Vertex &v1) {
        assert(v0 != v1);
        edges.push_back();
        halfedges.push_back();
        halfedges.push_back();

        Halfedge h(halfedges.size() - 2);
        Halfedge o(halfedges.size() - 1);

        set_vertex(h, v1);
        set_vertex(o, v0);
        return h;
    }


    Halfedge Graph::add_edge(const Vertex &v0, const Vertex &v1) {
        //check if edge exists between v0 and v1.
        auto h01 = find_halfedge(v0, v1);
        if (h01.is_valid()) {
            //edge exists, so just return it.
            return h01;
        }

        //there is no edge between v0 and v1 jet
        //check if v0 has an outgoing halfedge
        auto h0 = get_halfedge(v0);
        //check if v0 has an outgoing halfedge
        auto h1 = get_halfedge(v1);

        //make new edge between v0 and v1
        auto new_h = new_edge(v0, v1);
        auto new_o = get_opposite(new_h);

        if (h1.is_valid()) {
            auto p = get_prev(h1);
            set_next(p, new_o);
            set_next(new_h, h1);
        } else {
            set_next(new_h, new_o);
        }

        set_halfedge(v1, new_o);

        if (h0.is_valid()) {
            auto p = get_prev(h0);
            set_next(p, new_h);
            set_next(new_o, h0);
        } else {
            set_next(new_o, new_h);
        }

        set_halfedge(v0, new_h);
        return new_h;
    }

    Halfedge Graph::split(const Edge &e, const Vertex &v) {
        // before:
        //
        // v0      h0       v1
        //  o--------------->o
        //   <---------------
        //         o0
        //
        // after:
        //
        // v0  h0   v   h1   v1
        //  o------>o------->o
        //   <------ <-------
        //     o0       o1

        Halfedge h0 = get_halfedge(e, 0);
        Halfedge o0 = get_halfedge(e, 1);

        Vertex v1 = get_vertex(h0);

        Halfedge h0_next = get_next(h0);
        Halfedge o0_prev = get_prev(o0);

        Halfedge h1 = new_edge(v, v1);
        Halfedge o1 = get_opposite(h1);

        set_vertex(h0, v);
        set_halfedge(v, h1);

        set_next(h1, h0_next);
        set_next(h0, h1);

        set_next(o1, o0);
        set_next(o0_prev, o1);

        return h1;
    }

    void Graph::mark_deleted(const Edge &e) {
        if (e_deleted[e]) {
            return;
        }

        e_deleted[e] = true;
        ++edges.num_deleted;
        mark_deleted(get_halfedge(e, 0));
        mark_deleted(get_halfedge(e, 1));
    }

    /*void Graph::delete_edge(const Edge &e) {
        if (e_deleted[e]) return;

        Halfedge h0 = get_halfedge(e, 0);
        Halfedge h1 = get_halfedge(e, 1);

        Vertex from_v = get_vertex(h1);
        Vertex to_v = get_vertex(h0);

        if (halfedges.is_valid(h0)) {
            Halfedge p = get_prev(h0);
            Halfedge n = get_next(h1);
            if (find_halfedge(get_vertex(n), from_v).is_valid()) {
                set_next(p, n);
            }
        }
        if (halfedges.is_valid(h1)) {
            Halfedge p = get_prev(h1);
            Halfedge n = get_next(h0);
            if (find_halfedge(get_vertex(n), to_v).is_valid()) {
                set_next(p, n);
            }
        }

        mark_deleted(e);
    }*/

    void Graph::delete_edge(const Edge &e) {
        if (e_deleted[e]) return;

        Halfedge h = get_halfedge(e, 0);
        Halfedge o = get_halfedge(e, 1);

        Vertex v1 = get_vertex(h);
        Vertex v0 = get_vertex(o);

        if (halfedges.is_valid(h)) {
            Halfedge p = get_prev(h);
            Halfedge n = get_next(h);
            if (halfedges.is_valid(p) && halfedges.is_valid(n)) {
                set_next(p, n);
            }
        }
        if (halfedges.is_valid(o)) {
            Halfedge p = get_prev(o);
            Halfedge n = get_next(o);
            if (halfedges.is_valid(p) && halfedges.is_valid(n)) {
                set_next(p, n);
            }
        }

        mark_deleted(e);
    }

    void Graph::dfs_general_with_early_stopping(const Vertex &start,
                                             std::function<bool(const Vertex &)> vertex_action,
                                             std::function<bool(const Halfedge &)> halfedge_action) {
        std::vector<bool> visited(vertices.size(), false); // Vector to track visited vertices.
        std::stack<Vertex> stack;

        stack.push(start);

        while (!stack.empty()) {
            Vertex v = stack.top();
            stack.pop();
            if (!visited[v.idx()]) {
                visited[v.idx()] = true; // Mark as visited.
                if (vertex_action(v)) {
                    for (const Halfedge &h: get_halfedges(v)) {
                        if (halfedge_action(h)) {
                            Vertex neighbor = get_vertex(h);
                            if (!visited[neighbor.idx()]) {
                                stack.push(neighbor);
                            }
                        }
                    }
                }
            }
        }
        return visited;
    }

    void Graph::bfs_general_with_early_stopping(const Vertex &start,
                             std::function<bool(const Vertex &)> vertex_action,
                             std::function<bool(const Halfedge &)> halfedge_action) {
        std::vector<bool> visited(vertices.size(), false); // Vector to track visited vertices.
        std::queue<Vertex> queue;

        queue.push(start);
        visited[start.idx()] = true;

        while (!queue.empty()) {
            Vertex v = queue.front();
            queue.pop();

            if (!vertex_action(v)) return; // Stop traversal if vertex action returns false.

            for (const Halfedge &h : get_halfedges(v)) {
                if (!halfedge_action(h)) continue; // Skip this edge if halfedge action returns false.

                Vertex neighbor = get_vertex(h);
                if (!visited[neighbor.idx()]) {
                    visited[neighbor.idx()] = true; // Mark neighbor as visited.
                    queue.push(neighbor);
                }
            }
        }
        return visited;
    }

    //TODO: Move these to separate files

    EdgeProperty<Vector<unsigned int, 2> > Graph::get_edges() {
        EdgeProperty<Vector<unsigned int, 2> > indices = edge_property<Vector<unsigned int, 2> >("e:indices");
        for (const Edge &e: edges) {
            indices[e] = {get_vertex(get_halfedge(e, 0)).idx(), get_vertex(get_halfedge(e, 1)).idx()};
        }
        return indices;
    }
}
