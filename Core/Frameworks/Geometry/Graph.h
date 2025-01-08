//
// Created by alex on 08.01.25.
//

#ifndef ENGINE25_GRAPH_H
#define ENGINE25_GRAPH_H

#include "GeometricProperties.h"

namespace Bcg {
    class Graph {
    public:
        using VertexIterator = Iterator<Vertices, Vertex>;
        using VertexAroundVertexCirculator = VertexAroundVertexCirculatorBase<Graph>;
        using HalfedgeAroundVertexCirculator = HalfedgeAroundVertexCirculatorBase<Graph>;
        using EdgeAroundVertexCirculator = EdgeAroundVertexCirculatorBase<Graph>;

        Vertices vertices;
        Halfedges halfedges;
        Edges edges;

        VertexProperty<Vector<Real, 3>> positions;

        VertexProperty<bool> v_deleted;
        VertexProperty<Halfedge> v_connectivity;
        HalfedgeProperty<bool> h_deleted;
        EdgeProperty<bool> e_deleted;

        struct HalfedgeConnectivity {
            Vertex v;
            Halfedge nh;
            Halfedge ph;

            friend std::ostream &operator<<(std::ostream &os, const HalfedgeConnectivity &hc) {
                os << "v: " << hc.v.idx()
                   << "nh: " << hc.nh.idx()
                   << "ph: " << hc.ph.idx();
                return os;
            }
        };

        HalfedgeProperty<HalfedgeConnectivity> h_connectivity;

        Graph();

        virtual ~Graph() = default;

        Graph(const Graph &rhs) { operator=(rhs); }

        Graph &operator=(const Graph &rhs);

        Graph &assign(const Graph &rhs);

        void clear();

        void free_memory();

        void reserve(size_t nvertices);

        void garbage_collection();

        [[nodiscard]]  size_t n_vertices() const { return vertices.n_vertices(); }

        [[nodiscard]]  size_t n_halfedges() const { return halfedges.n_halfedges(); }

        [[nodiscard]]  size_t n_edges() const { return edges.n_edges(); }

        [[nodiscard]]  bool is_empty() const { return n_vertices() == 0; }

        [[nodiscard]]  bool is_deleted(const Vertex &v) const { return vertices.is_deleted(v); }

        [[nodiscard]]  bool is_deleted(const Halfedge &h) const { return halfedges.is_deleted(h); }

        [[nodiscard]]  bool is_deleted(const Edge &e) const { return edges.is_deleted(e); }

        [[nodiscard]]  bool is_valid(const Vertex &v) const { return vertices.is_valid(v); }

        [[nodiscard]]  bool is_valid(const Halfedge &h) const { return halfedges.is_valid(h); }

        [[nodiscard]]  bool is_valid(const Edge &e) const { return edges.is_valid(e); }

        [[nodiscard]]  bool has_garbage() const {
            return vertices.has_garbage() ||
                   halfedges.has_garbage() ||
                   edges.has_garbage();
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

         Vertex new_vertex() { return vertices.new_vertex(); }

        void mark_deleted(const Vertex &v);

        void mark_deleted(const Halfedge &h);

        void mark_deleted(const Edge &e);

        void delete_vertex(const Vertex &v);

        void delete_edge(const Edge &v);

        Vertex add_vertex(const Vector<Real, 3> &p);

        Halfedge new_edge(const Vertex &v0, const Vertex &v1);

        Halfedge add_edge(const Vertex &v0, const Vertex &v1);

        EdgeProperty<Vector<unsigned int, 2>> get_edges();

         bool is_isolated(const Vertex &v) const {
            return is_valid(get_halfedge(v)) && is_valid(get_opposite(get_halfedge(v)));
        }

         bool is_boundary(const Vertex &v) const {
            return is_boundary(get_halfedge(v));
        }

         bool is_boundary(const Halfedge &h) const {
            return get_next(h) == get_opposite(h);
        }

         bool is_boundary(const Edge &e) const {
            return is_boundary(get_halfedge(e, 0)) || is_boundary(get_halfedge(e, 1));
        }

        Halfedge find_halfedge(const Vertex &v0, const Vertex &v1) const;

         Halfedge get_opposite(const Halfedge &h) const {
            return Halfedge((h.idx() & 1) ? h.idx() - 1 : h.idx() + 1);
        }

         void set_halfedge(const Vertex &v, const Halfedge &h) {
            v_connectivity[v] = h;
        }

         Halfedge get_halfedge(const Vertex &v) const {
            return v_connectivity[v];
        }

         Halfedge get_halfedge(const Edge &e, int i) const {
            return Halfedge{(e.idx() << 1) + i};
        }

         Vertex get_vertex(const Edge &e, int i) const {
            return get_vertex(get_halfedge(e, i));
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
    };
}

#endif //ENGINE25_GRAPH_H
