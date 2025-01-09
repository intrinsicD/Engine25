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

        VertexProperty<Vector<Real, 3>> positions;
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

        // Vertex Methods

        [[nodiscard]] size_t n_vertices() const { return vertices.n_vertices(); }

        [[nodiscard]] bool is_deleted(const Vertex &v) const { return vertices.is_deleted(v); }

        [[nodiscard]] bool is_valid(const Vertex &v) const { return vertices.is_valid(v); }

        Vertex new_vertex() {
            vertices.push_back();
            return Vertex(vertices.size() - 1);
        }

        void mark_deleted(const Vertex &v);

        void delete_vertex(const Vertex &v);

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

        void mark_deleted(const Halfedge &h);

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

        Halfedge new_edge(const Vertex &v0, const Vertex &v1);

        void mark_deleted(const Edge &e);

        void delete_edge(const Edge &e);

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

        Face new_face() {
            faces.push_back();
            return Face(faces.size() - 1);
        }

        Face add_face(const Vertex &v0, const Vertex &v1, const Vertex &v2);

        Face add_face(const std::vector<Vertex> &f_vertices);

        void mark_deleted(const Face &f);

        void delete_face(const Face &f);

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
    };
}

#endif //MESH_H
