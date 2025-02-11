//
// Created by alex on 1/7/25.
//

#ifndef MESH_H
#define MESH_H

#include "Graph.h"

namespace Bcg {
    /**
     * @class Mesh
     * @brief A data structure combining graph-based topology with operations for handling polygonal meshes.
     *
     * The `Mesh` class inherits from `Graph` and builds on top of it to support polygonal mesh operations such as face
     * manipulation, vertex splitting, edge collapsing, and mesh triangulation. It provides a halfedge-centric implementation
     * to represent meshes, allowing efficient queries and modifications of mesh topology.
     *
     * It supports:
     * - Polygonal and triangular meshes.
     * - Topology changes such as edge flipping, splitting, and collapsing.
     * - Properties on vertices, halfedges, edges, and faces.
     *
     * ### Notable Functionalities
     * 1. Garbage collection for removing deleted elements.
     * 2. Triangulation of faces.
     * 3. Advanced geometric manipulations, including vertex and edge subdivision.
     *
     * The class maintains connectivity relationships and deleted status internally
     * through properties for vertices (`v_connectivity`), edges, and faces.
     */
    class Mesh : public Graph {
    public:
        using VertexAroundVertexCirculator = VertexAroundVertexCirculatorBase<Mesh>;
        using HalfedgeAroundVertexCirculator = HalfedgeAroundVertexCirculatorBase<Mesh>;
        using EdgeAroundVertexCirculator = EdgeAroundVertexCirculatorBase<Mesh>;
        using VertexAroundFaceCirculator = VertexAroundFaceCirculatorBase<Mesh>;
        using HalfedgeAroundFaceCirculator = HalfedgeAroundFaceCirculatorBase<Mesh>;
        using FaceAroundVertexCirculator = FaceAroundVertexCirculatorBase<Mesh>;

        using Graph::get_halfedge;
        using Graph::get_valence;
        using Graph::split;
        using Graph::is_boundary;

        Faces faces;

        FaceProperty<bool> f_deleted;
        FaceProperty<FaceConnectivity> f_connectivity;

        /**
         * @brief Default constructor for Mesh, initializes vertex, edge, face, and halfedge properties.
         */
        Mesh();

        /**
         * @brief Virtual destructor for extensibility.
         */
        ~Mesh() override = default;

        /**
         * @brief Copy constructor.
         * @param rhs Another Mesh instance to copy.
         */
        Mesh(const Mesh &rhs) { operator=(rhs); }

        /**
         * @brief Deep copy assignment operator for `Mesh`.
         * Copies the topology, geometry, and properties from the provided mesh `rhs` to this instance.
         * @param rhs The mesh to copy from.
         * @return Reference to this mesh after assignment.
         */
        Mesh &operator=(const Mesh &rhs);

        /**
         * @brief Deep copy assignment method.
         * @param rhs Another Mesh instance to copy.
         * @return Reference to this Mesh.
         */
        Mesh &assign(const Mesh &rhs);

        /**
         * @brief Deletes all elements (vertices, edges, faces) and their associated properties, resetting the mesh.
         */
        void clear() override;

        /**
         * @brief Releases unused memory.
         */
        void free_memory() override;

        /**
         * @brief Reserves memory for vertices, edges, and faces in the mesh.
         * @param nvertices The number of vertices to reserve space for.
         * @param nedges The number of edges to reserve space for.
         * @param nfaces The number of faces to reserve space for.
         */
        void reserve(size_t nvertices, size_t nedges, size_t nfaces);

        /**
         * @brief Compacts the mesh by removing deleted elements and updating connectivity.
         */
        void garbage_collection() override;

        /**
         * @brief Checks if the Mesh has any deleted vertices, edges, faces, or tets.
         * @return True if the Mesh has any deleted elements, false otherwise.
         */
        [[nodiscard]] bool has_garbage() const override {
            return vertices.has_garbage() ||
                   halfedges.has_garbage() ||
                   edges.has_garbage() ||
                   faces.has_garbage();
        }

        /**
         * @brief Checks if the mesh is a pure triangle mesh.
         * @return True if every face in the mesh has a valence of exactly 3.
         */
        bool is_triangle_mesh() const;

        /**
         * @brief Checks if the Mesh is a quad mesh.
         * @return True if the Mesh is a quad mesh, false otherwise.
         */
        bool is_quad_mesh() const;

        // -------------------------------------------------------------------------------------------------------------
        // Vertex Methods
        // -------------------------------------------------------------------------------------------------------------

        /**
         * @brief Checks if a vertex is manifold.
         * @param v Vertex to check.
         * @return True if the vertex is manifold, false otherwise.
         */
        [[nodiscard]] bool is_manifold(const Vertex &v) const;

        /**
         * @brief Checks if a vertex is isolated. An isolated vertex has no adjacent edges.
         * @param v Vertex to check.
         * @return True if the vertex is isolated, false otherwise.
         */
        [[nodiscard]] bool is_isolated(Vertex v) const { return !Graph::get_halfedge(v).is_valid(); }

        /**
         * @brief Checks if a vertex is on the boundary. The vertex is on the boundary if its outgoing halfedge is a boundary halfedge. It is adjusted like this during insertion and deletion.
         * @param v Vertex to check.
         * @return True if the vertex is on the boundary, false otherwise.
         */
        [[nodiscard]] bool is_boundary(const Vertex &v) const override {
            Halfedge h = Graph::get_halfedge(v);
            return !(h.is_valid() && get_face(h).is_valid());
        }

        /**
         * @brief Deletes a vertex and all incident faces.
         * @param v Vertex to delete.
         */
        void delete_vertex(const Vertex &v) override;


        /**
         * @brief Adjusts the outgoing halfedge of a vertex to be on the boundary.
         * @param v Vertex to adjust the outgoing halfedge for.
         */
        void adjust_outgoing_halfedge(const Vertex &v);

        /**
         * @brief Retrieves a circulator for the vertices around a vertex.
         * @param v Vertex to retrieve the circulator for.
         * @return Circulator for the vertices around the vertex.
         */
        VertexAroundVertexCirculator get_vertices(const Vertex &v) const {
            return {this, v};
        }

        /**
         * @brief Retrieves a circulator for the halfedges around a vertex.
         * @param v Vertex to retrieve the circulator for.
         * @return Circulator for the halfedges around the vertex.
         */
        HalfedgeAroundVertexCirculator get_halfedges(const Vertex &v) const {
            return {this, v};
        }

        /**
         * @brief Retrieves a circulator for the edges around a vertex.
         * @param v Vertex to retrieve the circulator for.
         * @return Circulator for the edges around the vertex.
         */
        EdgeAroundVertexCirculator get_edges(const Vertex &v) const {
            return {this, v};
        }

        /**
         * @brief Retrieves a circulator for the faces around a vertex.
         * @param v Vertex to retrieve the circulator for.
         * @return Circulator for the faces around the vertex.
         */
        FaceAroundVertexCirculator get_faces(const Vertex &v) const {
            return {this, v};
        }

        // -------------------------------------------------------------------------------------------------------------
        // Halfedge Methods
        // -------------------------------------------------------------------------------------------------------------

        /**
         * @brief Checks if a halfedge is on the boundary. A halfedge is on the boundary if its face is invalid.
         * @param h Halfedge to check.
         * @return True if the halfedge is on the boundary, false otherwise.
         */
        [[nodiscard]] bool is_boundary(const Halfedge &h) const override { return !get_face(h).is_valid(); }

        /**
         * @brief Retrieves the face of a halfedge.
         * @param h Halfedge to retrieve the face for.
         * @return The face of the halfedge.
         */
        Face get_face(const Halfedge &h) const { return h_connectivity[h].f; }

        /**
         * @brief Sets the face of a halfedge.
         * @param h Halfedge to set the face for.
         * @param f Face to set.
         */
        void set_face(const Halfedge &h, const Face &f) { h_connectivity[h].f = f; }

        /**
         * @brief Inserts a vertex along an existing halfedge.
         * @param h The halfedge to split by creating the new vertex.
         * @param v The vertex to insert.
         * @return A handle to the newly created opposite halfedge.
         *
         * ### Before and After:
         * ```
         * Before:
         *  v0 ----> v1
         *
         * After:
         *  v0 ---> v ---> v1
         * ```
         */
        Halfedge insert_vertex(const Halfedge &h, const Vertex &v);

        /**
         * @brief Inserts an edge between two halfedges.
         * @param h0 The first halfedge.
         * @param h1 The second halfedge.
         * @return A handle to the newly created halfedge.
         */
        Halfedge insert_edge(const Halfedge &h0, const Halfedge &h1);

        /**
         * @brief Checks if collapsing the given halfedge is allowed.
         * @param h The halfedge to check.
         * @return True if collapsing this halfedge does not create non-manifold topology, false otherwise.
         */
        [[nodiscard]] bool is_collapse_ok(const Halfedge &h) const;

        /**
         * @brief Collapses a halfedge.
         * @param h Halfedge to collapse.
         */
        void collapse(const Halfedge &h);

        /**
         * @brief Helper method to remove an edge.
         * @param h Halfedge to remove.
         */
        void remove_edge_helper(const Halfedge &h);

        /**
         * @brief Helper method to remove a loop.
         * @param h Halfedge to remove.
         */
        void remove_loop_helper(const Halfedge &h);

        // -------------------------------------------------------------------------------------------------------------
        // Edge Methods
        // -------------------------------------------------------------------------------------------------------------

        /**
         * @brief Checks if an edge can be deleted.
         * @param e Edge to check.
         * @return True if the edge can be deleted, false otherwise.
         */
        bool is_removal_ok(Edge e) const;

        /**
         * @brief Deletes an edge and all incident faces.
         * @param e Edge to delete.
         */
        void delete_edge(const Edge &e) override;

        /**
         * @brief Removes an edge.
         * @param e Edge to remove.
         * @return True if the edge was removed, false otherwise.
         */
        bool remove_edge(Edge e);

        /**
         * @brief Retrieves the face of an edge.
         * @param e Edge to retrieve the face for.
         * @param i Index of the face.
         * @return The face of the edge.
         */
        Face get_face(const Edge &e, int i) const {
            return get_face(get_halfedge(e, i));
        }

        /**
         * @brief Inserts a vertex into an edge.
         * @param e Edge to insert the vertex into.
         * @param p Position of the vertex to insert.
         * @return The new halfedge pointing to the inserted vertex from the target vertex of the edge.
         */
        Halfedge insert_vertex(const Edge &e, const Vector<Real, 3> &p) {
            return insert_vertex(e, add_vertex(p));
        }

        /**
         * @brief Inserts a vertex into an edge.
         * @param e Edge to insert the vertex into.
         * @param v Vertex to insert.
         * @return The new halfedge pointing to the inserted vertex from the target vertex of the edge.
         */
        Halfedge insert_vertex(const Edge &e, const Vertex &v) {
            return insert_vertex(get_halfedge(e, 0), v);
        }

        /**
         * @brief Finds an edge between two vertices.
         * @param start Start vertex of the edge.
         * @param end End vertex of the edge.
         * @return The edge between the two vertices if it exists, an invalid edge otherwise.
         */
        Edge find_edge(const Vertex &start, const Vertex &end) const;

        /**
         * @brief Checks if a flip operation is valid.
         * @param e Edge to flip.
         * @return True if the flip operation is valid, false otherwise.
         */
        [[nodiscard]] bool is_flip_ok(const Edge &e) const;

        /**
         * @brief Flips an edge.
         * @param e Edge to flip.
         */
        void flip(const Edge &e);

        /**
         *  @brief Splits an edge by inserting a vertex.
         *  @param e Edge to split.
         *  @param v Vertex to insert.
         *  @return The new Halfedge pointing to the inserted vertex from the target vertex of the edge.
         */
        Halfedge split(const Edge &e, const Vertex &v) override;

        // -------------------------------------------------------------------------------------------------------------
        // Face Methods
        // -------------------------------------------------------------------------------------------------------------

        /**
         * @brief Retrieves the number of faces.
         * @return Number of faces.
         */
        [[nodiscard]] size_t n_faces() const { return faces.n_faces(); }

        /**
         * @brief Checks if a face is deleted.
         * @param f Face to check.
         * @return True if deleted, false otherwise.
         */
        [[nodiscard]] bool is_deleted(const Face &f) const { return faces.is_deleted(f); }

        /**
         * @brief Checks if a face is valid.
         * @param f Face to check.
         * @return True if valid, false otherwise.
         */
        [[nodiscard]] bool is_valid(const Face &f) const { return faces.is_valid(f); }

        /**
         * @brief Checks if a face is on the boundary. A face is on the boundary if one of its halfedges is on the boundary.
         * @param f Face to check.
         * @return True if the face is on the boundary, false otherwise.
         */
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

        /**
         * @brief Creates a new face.
         * @return The new face.
         */
        Face new_face() {
            faces.push_back();
            return Face(faces.size() - 1);
        }

        /**
         * @brief Adds a triangle face.
         * @param v0 First vertex of the face.
         * @param v1 Second vertex of the face.
         * @param v2 Third vertex of the face.
         * @return The new face.
         */
        Face add_triangle(const Vertex &v0, const Vertex &v1, const Vertex &v2);

        /**
         * @brief Adds a quad face.
         * @param v0 First vertex of the face.
         * @param v1 Second vertex of the face.
         * @param v2 Third vertex of the face.
         * @param v3 Fourth vertex of the face.
         * @return The new face.
         */
        Face add_quad(const Vertex &v0, const Vertex &v1, const Vertex &v2, const Vertex &v3);

        /**
         * @brief Adds a face to the mesh using the given vertices.
         * @param f_vertices A vector of vertices defining the face. Must have at least 3 vertices.
         * @return The newly created face handle.
         */
        Face add_face(const std::vector<Vertex> &f_vertices);

        /**
         * @brief Marks a face as deleted.
         * @param f Face to mark as deleted.
         */
        void mark_deleted(const Face &f);

        /**
         * @brief Deletes a face.
         * @param f Face to delete.
         */
        void delete_face(const Face &f);

        /**
         * @brief Retrieves the halfedge of a face.
         * @param f Face to retrieve the halfedge from.
         * @return The halfedge of the face.
         */
        Halfedge get_halfedge(const Face &f) const { return f_connectivity[f].h; }

        /**
         * @brief Sets the halfedge of a face.
         * @param f Face to set the halfedge for.
         * @param h Halfedge to set.
         */
        void set_halfedge(const Face &f, const Halfedge &h) { f_connectivity[f].h = h; }

        /**
         * @brief Retrieves the valence of a face.
         * @param f Face to retrieve the valence for.
         * @return The valence of the face.
         */
        [[nodiscard]] size_t get_valence(const Face &f) const;

        /**
         * @brief Split the face \p f by first adding point \p p to the mesh and then inserting edges between \p p and
         * the vertices of \p f. For a triangle this is a standard one-to-three split.
         * @param f Face to split.
         * @param p Position of the point to split the
         * @return v The new vertex.
         */
        Vertex split(Face f, const Vector<Real, 3> &p) {
            Vertex v = add_vertex(p);
            split(f, v);
            return v;
        }

        /**
         * @brief Split the face \p f by inserting edges between \p v and the vertices of \p f. For a triangle this is
         * a standard one-to-three split.
         * @param f Face to split.
         * @param v Vertex to split the face with.
         */
        void split(Face f, Vertex v);

        /**
         * @brief Retrieves a circulator for the vertices around a face.
         * @param f Face to retrieve the circulator for.
         * @return Circulator for the vertices around the face.
         */
        VertexAroundFaceCirculator get_vertices(const Face &f) const {
            return {this, f};
        }

        /**
         * @brief Retrieves a circulator for the halfedges around a face.
         * @param f Face to retrieve the circulator for.
         * @return Circulator for the halfedges around the face.
         */
        HalfedgeAroundFaceCirculator get_halfedges(const Face &f) const {
            return {this, f};
        }

        /**
         * @brief Adds an edge property to the Mesh.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @param t Default value of the property.
         * @return The added edge property.
         */
        template<class T>
        FaceProperty<T> add_face_property(const std::string &name,
                                          const T t = T()) {
            return FaceProperty<T>(faces.add<T>(name, t));
        }

        /**
         * @brief Retrieves a face property by name.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @return The face property if it exists.
         */
        template<class T>
        FaceProperty<T> get_face_property(const std::string &name) const {
            return FaceProperty<T>(faces.get<T>(name));
        }

        /**
         * @brief Retrieves or adds a face property.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @param t Default value of the property.
         * @return The face property.
         */
        template<class T>
        FaceProperty<T> face_property(const std::string &name, const T t = T()) {
            return FaceProperty<T>(faces.get_or_add<T>(name, t));
        }

        /**
         * @brief Removes a face property.
         * @tparam T Type of the property.
         * @param p Face property to remove.
         */
        template<class T>
        void remove_face_property(FaceProperty<T> &p) {
            faces.remove(p);
        }

        /**
         * @brief Checks if a face property exists.
         * @param name Name of the property.
         * @return True if the property exists, false otherwise.
         */
        [[nodiscard]] bool has_face_property(const std::string &name) const {
            return faces.exists(name);
        }

        /**
         * @brief Triangulates a face.
         * @param f Face to triangulate.
         */
        void triangulate(const Face &f);

        /**
         * @brief Triangulates all non-triangular faces in the mesh.
         */
        void triangulate();

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
