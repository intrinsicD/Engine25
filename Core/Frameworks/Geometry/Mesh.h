//
// Created by alex on 1/7/25.
//

#ifndef MESH_H
#define MESH_H

#include "GeometricProperties.h"

namespace Bcg {
    class Mesh {
    public:
        using VertexAroundVertexCirculator = VertexAroundVertexCirculatorBase<Mesh>;
        using HalfedgeAroundVertexCirculator = HalfedgeAroundVertexCirculatorBase<Mesh>;
        using EdgeAroundVertexCirculator = EdgeAroundVertexCirculatorBase<Mesh>;
        using VertexAroundFaceCirculator = VertexAroundFaceCirculatorBase<Mesh>;
        using HalfedgeAroundFaceCirculator = HalfedgeAroundFaceCirculatorBase<Mesh>;
        using FaceAroundVertexCirculator = FaceAroundVertexCirculatorBase<Mesh>;

        Vertices vertices;
        Halfedges halfedges;
        Edges edges;
        Faces faces;
        Tets tets;

        VertexProperty<Vector<Real, 3> > positions;
        VertexProperty<bool> v_deleted;
        VertexProperty<VertexConnectivity> v_connectivity;

        HalfedgeProperty<bool> h_deleted;
        HalfedgeProperty<HalfedgeConnectivity> h_connectivity;

        EdgeProperty<bool> e_deleted;
        FaceProperty<bool> f_deleted;
        FaceProperty<FaceConnectivity> f_connectivity;

        TetProperty<bool> t_deleted;
        TetProperty<TetConnectivity> t_connectivity;

        Mesh();

        virtual ~Mesh() = default;

        Mesh(const Mesh &rhs) { operator=(rhs); }

        Mesh &operator=(const Mesh &rhs);

        Mesh &assign(const Mesh &rhs);

        void clear();

        void free_memory();

        void reserve(size_t nvertices, size_t nedges, size_t nfaces, size_t ntets = 0);

        void garbage_collection();

        [[nodiscard]] bool is_empty() const { return n_vertices() == 0; }

        [[nodiscard]] bool has_garbage() const {
            return vertices.has_garbage() ||
                   halfedges.has_garbage() ||
                   edges.has_garbage() ||
                   faces.has_garbage() ||
                   tets.has_garbage();
        }

        bool is_triangle_mesh() const;

        bool is_quad_mesh() const;

        // Vertex Methods

        [[nodiscard]] size_t n_vertices() const { return vertices.n_vertices(); }

        [[nodiscard]] bool is_deleted(const Vertex &v) const { return vertices.is_deleted(v); }

        [[nodiscard]] bool is_valid(const Vertex &v) const { return vertices.is_valid(v); }

        [[nodiscard]] bool is_manifold(const Vertex &v) const;

        [[nodiscard]] bool is_isolated(Vertex v) const { return !get_halfedge(v).is_valid(); }

        [[nodiscard]] bool is_boundary(const Vertex &v) const {
            Halfedge h = get_halfedge(v);
            return !(h.is_valid() && get_face(h).is_valid());
        }

        Vertex new_vertex() {
            vertices.push_back();
            return Vertex(vertices.size() - 1);
        }

        Vertex add_vertex(const Vector<Real, 3> &p);

        void mark_deleted(const Vertex &v);

        void delete_vertex(const Vertex &v);

        Halfedge get_halfedge(const Vertex &v) const { return v_connectivity[v].h; }

        void set_halfedge(const Vertex &v, const Halfedge &h) { v_connectivity[v].h = h; }

        [[nodiscard]] size_t get_valence(const Vertex &v) const;

        void adjust_outgoing_halfedge(Vertex v);

        VertexAroundVertexCirculator get_vertices(const Vertex &v) const {
            return {this, v};
        }

        HalfedgeAroundVertexCirculator get_halfedges(const Vertex &v) const {
            return {this, v};
        }

        EdgeAroundVertexCirculator get_edges(const Vertex &v) const {
            return {this, v};
        }

        FaceAroundVertexCirculator get_faces(const Vertex &v) const {
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

        [[nodiscard]] bool is_boundary(const Halfedge &h) const { return !get_face(h).is_valid(); }

        void mark_deleted(const Halfedge &h);

        Vertex get_vertex(const Halfedge &h) const { return h_connectivity[h].v; }

        void set_vertex(const Halfedge &h, const Vertex &v) { h_connectivity[h].v = v; }

        Face get_face(const Halfedge &h) const { return h_connectivity[h].f; }

        void set_face(const Halfedge &h, const Face &f) { h_connectivity[h].f = f; }

        Halfedge get_next(const Halfedge &h) const { return h_connectivity[h].nh; }

        void set_next(const Halfedge &h, const Halfedge &nh) {
            h_connectivity[h].nh = nh;
            h_connectivity[nh].ph = h;
        }

        Halfedge get_prev(const Halfedge &h) const { return h_connectivity[h].ph; }

        void set_prev(const Halfedge &h, const Halfedge &ph) {
            h_connectivity[h].ph = ph;
            h_connectivity[ph].nh = h;
        }

        Halfedge get_opposite(const Halfedge &h) const {
            return Halfedge((h.idx() & 1) ? h.idx() - 1 : h.idx() + 1);
        }

        Halfedge rotate_cw(const Halfedge &h) const { return get_next(get_opposite(h)); }

        Halfedge rotate_ccw(const Halfedge &h) const { return get_opposite(get_prev(h)); }

        Edge get_edge(const Halfedge &h) const { return Edge(h.idx() >> 1); }

        Halfedge insert_vertex(Halfedge h0, Vertex v);

        Halfedge find_halfedge(Vertex start, Vertex end) const;

        bool is_collapse_ok(Halfedge v0v1) const;

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
        void remove_halfedge_property(HalfedgeProperty<T> &p) {
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

        void mark_deleted(const Edge &e);

        void delete_edge(const Edge &e);

        Halfedge get_halfedge(const Edge &e, int i) const {
            return Halfedge{(e.idx() << 1) + i};
        }

        Vertex get_vertex(const Edge &e, int i) const {
            return get_vertex(get_halfedge(e, i));
        }

        Face get_face(const Edge &e, int i) const {
            return get_face(get_halfedge(e, i));
        }

        Halfedge insert_vertex(const Edge &e, const Vector<Real, 3> &p) {
            return insert_vertex(e, add_vertex(p));
        }

        Halfedge insert_vertex(Edge e, Vertex v) {
            return insert_vertex(get_halfedge(e, 0), v);
        }

        Edge find_edge(Vertex a, Vertex b) const;

        bool is_flip_ok(Edge e) const;

        void flip(Edge e);

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

        // Face Methods

        [[nodiscard]] size_t n_faces() const { return faces.n_faces(); }

        [[nodiscard]] bool is_deleted(const Face &f) const { return faces.is_deleted(f); }

        [[nodiscard]] bool is_valid(const Face &f) const { return faces.is_valid(f); }

        [[nodiscard]] bool is_boundary(const Face &f) const {
            Halfedge h = get_halfedge(f);
            Halfedge hh = h;
            do {
                if (is_boundary(get_opposite(h))) {
                    return true;
                }
                h = get_next(h);
            } while (h != hh);
            return false;
        }

        Face new_face() {
            faces.push_back();
            return Face(faces.size() - 1);
        }

        Face add_triangle(const Vertex &v0, const Vertex &v1, const Vertex &v2);

        Face add_quad(const Vertex &v0, const Vertex &v1, const Vertex &v2, const Vertex &v3);

        Face add_face(const std::vector<Vertex> &f_vertices);

        void mark_deleted(const Face &f);

        void delete_face(const Face &f);

        Halfedge get_halfedge(const Face &f) const { return f_connectivity[f].h; }

        void set_halfedge(const Face &f, const Halfedge &h) { f_connectivity[f].h = h; }

        [[nodiscard]] size_t get_valence(const Face &f) const;

        VertexAroundFaceCirculator get_vertices(const Face &f) const {
            return {this, f};
        }

        HalfedgeAroundFaceCirculator get_halfedges(const Face &f) const {
            return {this, f};
        }

        template<class T>
        FaceProperty<T> add_face_property(const std::string &name,
                                          const T t = T()) {
            return FaceProperty<T>(faces.add<T>(name, t));
        }

        template<class T>
        FaceProperty<T> get_face_property(const std::string &name) const {
            return FaceProperty<T>(faces.get<T>(name));
        }

        template<class T>
        FaceProperty<T> face_property(const std::string &name, const T t = T()) {
            return FaceProperty<T>(faces.get_or_add<T>(name, t));
        }

        template<class T>
        void remove_face_property(FaceProperty<T> &p) {
            faces.remove(p);
        }

        [[nodiscard]] bool has_face_property(const std::string &name) const {
            return faces.exists(name);
        }

        // Tet Methods

        [[nodiscard]] size_t n_tets() const { return tets.n_tets(); }

        [[nodiscard]] bool is_deleted(const Tet &t) const { return tets.is_deleted(t); }

        [[nodiscard]] bool is_valid(const Tet &t) const { return tets.is_valid(t); }

        Tet new_tet() {
            tets.push_back();
            return Tet(tets.size() - 1);
        }

        Tet add_tet(const Vertex &v0, const Vertex &v1, const Vertex &v2, const Vertex &v3);

        void mark_deleted(const Tet &t);

        void delete_tet(const Tet &t);

        template<class T>
        TetProperty<T> add_tet_property(const std::string &name,
                                        const T t = T()) {
            return TetProperty<T>(tets.add<T>(name, t));
        }

        template<class T>
        TetProperty<T> get_tet_property(const std::string &name) const {
            return TetProperty<T>(tets.get<T>(name));
        }

        template<class T>
        TetProperty<T> tet_property(const std::string &name, const T t = T()) {
            return TetProperty<T>(tets.get_or_add<T>(name, t));
        }

        template<class T>
        void remove_tet_property(TetProperty<T> &p) {
            tets.remove(p);
        }

        [[nodiscard]] bool has_tet_property(const std::string &name) const {
            return tets.exists(name);
        }
    private:
        using NextCacheEntry = std::pair<Halfedge, Halfedge>;
        using NextCache = std::vector<NextCacheEntry>;
        std::vector<Vertex> m_add_face_vertices;
        std::vector<Halfedge> m_add_face_halfedges;
        std::vector<bool> m_add_face_is_new;
        std::vector<bool> m_add_face_needs_adjust;
        NextCache m_add_face_next_cache;
    };
}

#endif //MESH_H
