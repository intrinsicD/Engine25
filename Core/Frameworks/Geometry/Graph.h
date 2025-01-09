//
// Created by alex on 08.01.25.
//

#ifndef ENGINE25_GRAPH_H
#define ENGINE25_GRAPH_H

#include "GeometricProperties.h"

namespace Bcg {
    class Graph {
    public:
        using VertexAroundVertexCirculator = VertexAroundVertexCirculatorBase<Graph>;
        using HalfedgeAroundVertexCirculator = HalfedgeAroundVertexCirculatorBase<Graph>;
        using EdgeAroundVertexCirculator = EdgeAroundVertexCirculatorBase<Graph>;

        Vertices vertices;
        Halfedges halfedges;
        Edges edges;

        VertexProperty<Vector<Real, 3>> positions;
        VertexProperty<bool> v_deleted;
        VertexProperty<VertexConnectivity> v_connectivity;

        HalfedgeProperty<bool> h_deleted;
        HalfedgeProperty<HalfedgeConnectivity> h_connectivity;

        EdgeProperty<bool> e_deleted;

        Graph();

        virtual ~Graph() = default;

        Graph(const Graph &rhs) { operator=(rhs); }

        Graph &operator=(const Graph &rhs);

        Graph &assign(const Graph &rhs);

        void clear();

        void free_memory();

        void reserve(size_t nvertices, size_t nedges);

        void garbage_collection();

        [[nodiscard]] bool is_empty() const { return n_vertices() == 0; }

        [[nodiscard]] bool has_garbage() const {
            return vertices.has_garbage() ||
                   halfedges.has_garbage() ||
                   edges.has_garbage();
        }

        // Vertex Methods

        [[nodiscard]] size_t n_vertices() const { return vertices.n_vertices(); }

        [[nodiscard]] bool is_deleted(const Vertex &v) const { return vertices.is_deleted(v); }

        [[nodiscard]] bool is_valid(const Vertex &v) const { return vertices.is_valid(v); }

        [[nodiscard]] bool is_isolated(const Vertex &v) const { return !is_valid(get_halfedge(v)); }

        // check only the incoming halfedge because if v is a boundary vertex then the outgoing halfedge is the next
        // halfedge of the incoming halfedge (its build that way)
        [[nodiscard]] bool is_boundary(const Vertex &v) const { return is_boundary(get_opposite(get_halfedge(v))); }

        Vertex new_vertex() {
            vertices.push_back();
            return Vertex(vertices.size() - 1);
        }

        Vertex add_vertex(const Vector<Real, 3> &p);

        void mark_deleted(const Vertex &v);

        void delete_vertex(const Vertex &v);

        void set_halfedge(const Vertex &v, const Halfedge &h) {
            v_connectivity[v].h = h;
        }

        Halfedge get_halfedge(const Vertex &v) const {
            return v_connectivity[v].h;
        }

        size_t get_valence(const Vertex &v) const;

        VertexAroundVertexCirculator get_vertices(const Vertex &v) const {
            return {this, v};
        }

        HalfedgeAroundVertexCirculator get_halfedges(const Vertex &v) const {
            return {this, v};
        }

        EdgeAroundVertexCirculator get_edges(const Vertex &v) const {
            return {this, v};
        }

        template<class T>
        VertexProperty<T> add_vertex_property(const std::string &name,
                                              const T t = T()) {
            return VertexProperty<T>(vertices.add<T>(name, t));
        }

        template<class T>
        VertexProperty<T> get_vertex_property(const std::string &name) const {
            return VertexProperty<T>(vertices.get<T>(name));
        }

        template<class T>
        VertexProperty<T> vertex_property(const std::string &name, const T t = T()) {
            return VertexProperty<T>(vertices.get_or_add<T>(name, t));
        }

        template<class T>
        void remove_vertex_property(VertexProperty<T> &p) {
            vertices.remove(p);
        }

        [[nodiscard]] bool has_vertex_property(const std::string &name) const {
            return vertices.exists(name);
        }

        // Halfedge Methods

        [[nodiscard]] size_t n_halfedges() const { return halfedges.n_halfedges(); }

        [[nodiscard]] bool is_deleted(const Halfedge &h) const { return halfedges.is_deleted(h); }

        [[nodiscard]] bool is_valid(const Halfedge &h) const { return halfedges.is_valid(h); }

        [[nodiscard]] bool is_boundary(const Halfedge &h) const { return get_next(h) == get_opposite(h); }

        void mark_deleted(const Halfedge &h);

        Halfedge find_halfedge(const Vertex &v0, const Vertex &v1) const;

        Halfedge get_opposite(const Halfedge &h) const {
            return Halfedge((h.idx() & 1) ? h.idx() - 1 : h.idx() + 1);
        }

        void set_vertex(const Halfedge &h, const Vertex &v) {
            h_connectivity[h].v = v;
        }

        Vertex get_vertex(const Halfedge &h) const {
            return h_connectivity[h].v;
        }

        Halfedge get_next(const Halfedge &h) const {
            return h_connectivity[h].nh;
        }

        void set_next(const Halfedge &h, const Halfedge &nh) {
            h_connectivity[h].nh = nh;
            h_connectivity[h].ph = h;
        }

        Halfedge get_prev(const Halfedge &h) const {
            return h_connectivity[h].ph;
        }

        Halfedge rotate_cw(const Halfedge &h) const {
            return get_next(get_opposite(h));
        }

        Halfedge rotate_ccw(const Halfedge &h) const {
            return get_opposite(get_prev(h));
        }

        Edge get_edge(const Halfedge &h) const {
            return Edge(h.idx() >> 1);
        }

        template<class T>
        HalfedgeProperty<T> add_halfedge_property(const std::string &name,
                                                  const T t = T()) {
            return HalfedgeProperty<T>(halfedges.add<T>(name, t));
        }

        template<class T>
        HalfedgeProperty<T> get_halfedge_property(const std::string &name) const {
            return HalfedgeProperty<T>(halfedges.get<T>(name));
        }

        template<class T>
        HalfedgeProperty<T> halfedge_property(const std::string &name, const T t = T()) {
            return HalfedgeProperty<T>(halfedges.get_or_add<T>(name, t));
        }

        template<class T>
        void remove_vertex_property(HalfedgeProperty<T> &p) {
            halfedges.remove(p);
        }

        [[nodiscard]] bool has_halfedge_property(const std::string &name) const {
            return halfedges.exists(name);
        }

        // Edge Methods

        [[nodiscard]] size_t n_edges() const { return edges.n_edges(); }

        [[nodiscard]] bool is_deleted(const Edge &e) const { return edges.is_deleted(e); }

        [[nodiscard]] bool is_valid(const Edge &e) const { return edges.is_valid(e); }

        [[nodiscard]] bool is_boundary(const Edge &e) const {
            return is_boundary(get_halfedge(e, 0)) || is_boundary(get_halfedge(e, 1));
        }

        Halfedge new_edge(const Vertex &v0, const Vertex &v1);

        Halfedge add_edge(const Vertex &v0, const Vertex &v1);

        void mark_deleted(const Edge &e);

        void delete_edge(const Edge &e);

        Halfedge get_halfedge(const Edge &e, int i) const {
            return Halfedge{(e.idx() << 1) + i};
        }

        Vertex get_vertex(const Edge &e, int i) const {
            return get_vertex(get_halfedge(e, i));
        }

        template<class T>
        EdgeProperty<T> add_edge_property(const std::string &name,
                                          const T t = T()) {
            return EdgeProperty<T>(edges.add<T>(name, t));
        }

        template<class T>
        EdgeProperty<T> get_edge_property(const std::string &name) const {
            return EdgeProperty<T>(edges.get<T>(name));
        }

        template<class T>
        EdgeProperty<T> edge_property(const std::string &name, const T t = T()) {
            return EdgeProperty<T>(edges.get_or_add<T>(name, t));
        }

        template<class T>
        void remove_edge_property(HalfedgeProperty<T> &p) {
            edges.remove(p);
        }

        [[nodiscard]] bool has_edge_property(const std::string &name) const {
            return edges.exists(name);
        }

        //TODO: Move these to separate files

        EdgeProperty<Vector<unsigned int, 2>> get_edges();

        template<typename T, int N>
        Vector<T, N> vector(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N>> &pos) const {
            return pos[v1] - pos[v0];
        }

        template<typename T, int N>
        Vector<T, N> vector(const Edge &e, const VertexProperty<Vector<T, N>> &pos) const {
            return vector(get_vertex(e, 0), get_vertex(e, 1), pos);
        }

        template<typename T, int N>
        Vector<T, N> vector(const Halfedge &h, const VertexProperty<Vector<T, N>> &pos) const {
            return vector(get_edge(h), pos);
        }

        template<typename T, int N>
        Real length(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N>> &pos) const {
            return vector(v0, v1, pos).norm();
        }

        template<typename T, int N>
        Real length(const Edge &e, const VertexProperty<Vector<T, N>> &pos) const {
            return vector(e, pos).norm();
        }

        template<typename T, int N>
        Real length(const Halfedge &h, const VertexProperty<Vector<T, N>> &pos) const {
            return vector(h, pos).norm();
        }

        template<typename T, int N>
        Vector<T, N> center(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N>> &pos) const {
            return (pos[v0] + pos[v1]) / 2;
        }

        template<typename T, int N>
        Vector<T, N> center(const Edge &e, const VertexProperty<Vector<T, N>> &pos) const {
            return center(get_vertex(e, 0), get_vertex(e, 1), pos);
        }

        template<typename T, int N>
        Vector<T, N> center(const Halfedge &h, const VertexProperty<Vector<T, N>> &pos) const {
            return center(get_edge(h), pos);
        }
    };
}

#endif //ENGINE25_GRAPH_H
