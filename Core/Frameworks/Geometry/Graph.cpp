//
// Created by alex on 08.01.25.
//

#include "Graph.h"

namespace Bcg {
    Graph::Graph() : vertices(), halfedges(), edges() {
        // link properties to containers
        positions = vertices.vertex_property<Vector<Real, 3>>("v:position");
        v_connectivity = vertices.vertex_property<VertexConnectivity>("v:connectivity");
        h_connectivity = halfedges.halfedge_property<HalfedgeConnectivity>("h:connectivity");
        v_deleted = vertices.vertex_property<bool>("v:deleted", false);
        h_deleted = halfedges.halfedge_property<bool>("h:deleted", false);
        e_deleted = edges.edge_property<bool>("e:deleted", false);
    }

    Graph &Graph::operator=(const Graph &rhs) {
        if (this != &rhs) {
            // deep copy of property containers
            vertices = rhs.vertices;
            halfedges = rhs.halfedges;
            edges = rhs.edges;

            // link properties from copied containers
            positions = vertices.vertex_property<Vector<Real, 3>>("v:position");

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

        positions = vertices.vertex_property<Vector<Real, 3>>("v:position", Vector<Real, 3>::Zero());
        v_connectivity = vertices.vertex_property<VertexConnectivity>("v:connectivity");
        h_connectivity = halfedges.halfedge_property<HalfedgeConnectivity>("h:connectivity");
    }

    void Graph::free_memory() {
        vertices.free_memory();
        halfedges.free_memory();
        edges.free_memory();
    }

    void Graph::reserve(size_t nvertices, size_t nedges) {
        vertices.reserve(nvertices);
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

    Vertex Graph::add_vertex(const Vector<Real, 3> &p) {
        Vertex v = new_vertex();
        if (v.is_valid()) {
            positions[v] = p;
        }
        return v;
    }

    void Graph::mark_deleted(const Vertex &v) {
        if (v_deleted[v]) {
            return;
        }

        v_deleted[v] = true;
        ++vertices.num_deleted;
    }


    void Graph::delete_vertex(const Vertex &v) {
        if (v_deleted[v]) {
            return;
        }

        for (const Edge &e: get_edges(v)) {
            delete_edge(e);
        }

        mark_deleted(v);
    }

    size_t Graph::get_valence(const Vertex &v) const {
        size_t valence = 0;
        for (const Edge &e: get_edges(v)) {
            valence++;
        }
        return valence;
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
        assert(is_valid(v0) && is_valid(v1));

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

    /*Halfedge Graph::add_edge(const Vertex &v0, const Vertex &v1) {
        //check if edge exists between v0 and v1.
        Halfedge h01 = find_halfedge(v0, v1);
        if (h01.is_valid()) {
            //edge exists, so just return it.
            return h01;
        }

        //there is no edge between v0 and v1 jet
        //check if v0 has an outgoing halfedge
        Halfedge h0 = get_halfedge(v0);
        //check if v0 has an outgoing halfedge
        Halfedge h1 = get_halfedge(v1);

        //make new edge between v0 and v1
        Halfedge new_h = new_edge(v0, v1);
        Halfedge new_o = get_opposite(new_h);

        if (h1.is_valid()) {
            Halfedge p = get_prev(h1);
            set_next(p, new_o);
            set_next(new_h, h1);
        } else {
            set_next(new_h, new_o);
        }

        set_halfedge(v1, new_o);

        if (h0.is_valid()) {
            Halfedge p = get_prev(h0);
            set_next(p, new_h);
            set_next(new_o, h0);
        } else {
            set_next(new_o, new_h);
        }

        set_halfedge(v0, new_h);

        return new_h;
    }*/

    Halfedge Graph::add_edge(const Vertex &v0, const Vertex &v1) {
        //check if edge exists between v0 and v1.
        Halfedge h01 = find_halfedge(v0, v1);
        if (h01.is_valid()) {
            //edge exists, so just return it.
            return h01;
        }

        //make new edge between v0 and v1
        Halfedge h = new_edge(v0, v1);

        //there is no edge between v0 and v1 jet
        //check if v0 has an outgoing halfedge
        Halfedge h0 = get_halfedge(v0);

        if (h0.is_valid()) {
            Halfedge p = get_prev(h0);
            Halfedge n = get_next(h0);

            if (n.is_valid()) {
                set_next(h, n);
            }
            if (p.is_valid()) {
                set_next(p, h);
            }
        }
        //check if v1 has an outgoing halfedge
        Halfedge h1 = get_halfedge(v1);

        if (h1.is_valid()) {
            Halfedge p = get_prev(h1);
            Halfedge n = get_next(h1);

            if (n.is_valid()) {
                set_next(h, n);
            }
            if (p.is_valid()) {
                set_next(p, h);
            }
        }

        set_halfedge(v0, h);
        set_halfedge(v1, get_opposite(h));

        return h;
    }

    void Graph::mark_deleted(const Edge &e) {
        if (e_deleted[e]) {
            return;
        }

        e_deleted[e] = true;
        mark_deleted(get_halfedge(e, 0));
        mark_deleted(get_halfedge(e, 1));
        ++edges.num_deleted;
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

    //TODO: Move these to separate files

    EdgeProperty<Vector<unsigned int, 2> > Graph::get_edges() {
        EdgeProperty<Vector<unsigned int, 2> > indices = edge_property<Vector<unsigned int, 2> >("e:indices");
        for (const Edge &e: edges) {
            indices[e] = {get_vertex(get_halfedge(e, 0)).idx(), get_vertex(get_halfedge(e, 1)).idx()};
        }
        return indices;
    }


}
