//
// Created by alex on 08.01.25.
//

#include "Graph.h"

namespace Bcg {
    Graph::Graph() {

    }

    Graph &Graph::operator=(const Graph &rhs) {

    }

    Graph &Graph::assign(const Graph &rhs) {

    }

    void Graph::clear() {

    }

    void Graph::free_memory() {

    }

    void Graph::reserve(size_t nvertices) {

    }

    void Graph::garbage_collection() {

    }

    Vertex Graph::add_vertex(const Vector<Real, 3> &p) {

    }

    void Graph::mark_deleted(const Vertex &v) {

    }

    void Graph::mark_deleted(const Halfedge &h) {

    }

    void Graph::mark_deleted(const Edge &e) {

    }

    void Graph::delete_vertex(const Vertex &v) {

    }

    void Graph::delete_edge(const Edge &v) {

    }

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

    Property<Vector<unsigned int, 2>> Graph::get_edges() {

    }

    Halfedge Graph::find_halfedge(const Vertex &v0, const Vertex &v1) const {
        if (halfedges.is_valid(get_halfedge(v0))) {
            for (const auto h: get_halfedges(v0)) {
                if (get_vertex(h) == v1) {
                    return h;
                }
            }
        }

        return {};
    }

    size_t Graph::get_valence(const Vertex &v) const {

    }
}