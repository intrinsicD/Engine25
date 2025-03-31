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
    class Mesh{
    public:
        using VertexAroundVertexCirculator = VertexAroundVertexCirculatorBase<Mesh>;
        using HalfedgeAroundVertexCirculator = HalfedgeAroundVertexCirculatorBase<Mesh>;
        using EdgeAroundVertexCirculator = EdgeAroundVertexCirculatorBase<Mesh>;
        using VertexAroundFaceCirculator = VertexAroundFaceCirculatorBase<Mesh>;
        using HalfedgeAroundFaceCirculator = HalfedgeAroundFaceCirculatorBase<Mesh>;
        using FaceAroundVertexCirculator = FaceAroundVertexCirculatorBase<Mesh>;

        VertexContainer vertices;       ///< Container for managing vertex data.
        HalfedgeContainer halfedges;    ///< Container for managing halfedge data.
        EdgeContainer edges;            ///< Container for managing edge data.
        FaceContainer faces;            ///< Container for managing face data.

        VertexProperty<bool> v_deleted; ///< Deletion flag for vertices.
        VertexProperty<VertexConnectivity> v_connectivity; ///< Connectivity information for vertices.

        HalfedgeProperty<bool> h_deleted; ///< Deletion flag for halfedges.
        HalfedgeProperty<HalfedgeConnectivity> h_connectivity; ///< Connectivity information for halfedges.

        EdgeProperty<bool> e_deleted;   ///< Deletion flag for edges.
        EdgeProperty<Halfedge> e_direction; ///< Direction of the edge (halfedge) in the graph.

        FaceProperty<bool> f_deleted;
        FaceProperty<FaceConnectivity> f_connectivity;

        /**
         * @brief Default constructor for Mesh, initializes vertex, edge, face, and halfedge properties.
         */
        Mesh();

        Mesh(VertexContainer vertices, HalfedgeContainer halfedges, EdgeContainer edges, FaceContainer faces);

        /**
         * @brief Virtual destructor for extensibility.
         */
        virtual ~Mesh() = default;

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
        virtual void clear();

        /**
         * @brief Releases unused memory.
         */
        virtual void free_memory();

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
        virtual void garbage_collection();

        /**
         * @brief Checks if the Mesh has any deleted vertices, edges, faces, or tets.
         * @return True if the Mesh has any deleted elements, false otherwise.
         */
        [[nodiscard]] bool has_garbage() const {
            return vertices.has_garbage() ||
                   halfedges.has_garbage() ||
                   edges.has_garbage() ||
                   faces.has_garbage();
        }

        /**
         * @brief Returns the total number of undeleted vertices.
         * @return Number of undeleted vertices.
         */
        [[nodiscard]] size_t n_vertices() const { return vertices.n_vertices(); }

        /**
         * @brief Checks if the PointCloud is empty.
         * @return True if empty, false otherwise.
         */
        [[nodiscard]] bool is_empty() const { return n_vertices() == 0; }

        /**
         * @brief Checks if a vertex is deleted.
         * @param v Vertex to check.
         * @return True if deleted, false otherwise.
         */
        [[nodiscard]] bool is_deleted(const Vertex &v) const { return vertices.is_deleted(v); }

        /**
         * @brief Checks if a vertex is valid.
         * @param v Vertex to check.
         * @return True if valid, false otherwise.
         */
        [[nodiscard]] bool is_valid(const Vertex &v) const { return vertices.is_valid(v); }


        /**
         * @brief Adds a property to vertices.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @param t Default value for the property.
         * @return The added property.
         */
        template<class T>
        VertexProperty<T> add_vertex_property(const std::string &name,
                                              const T t = T()) {
            return VertexProperty<T>(vertices.add<T>(name, t));
        }

        /**
         * @brief Retrieves a vertex property by name.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @return The property if it exists.
         */
        template<class T>
        VertexProperty<T> get_vertex_property(const std::string &name) const {
            return VertexProperty<T>(vertices.get<T>(name));
        }

        /**
         * @brief Retrieves or adds a vertex property.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @param t Default value for the property.
         * @return The property.
         */
        template<class T>
        VertexProperty<T> vertex_property(const std::string &name, const T t = T()) {
            return VertexProperty<T>(vertices.get_or_add<T>(name, t));
        }

        /**
         * @brief Removes a vertex property.
         * @tparam T Type of the property.
         * @param p Property to remove.
         */
        template<class T>
        void remove_vertex_property(VertexProperty<T> &p) {
            vertices.remove(p);
        }

        /**
         * @brief Checks if a vertex property exists.
         * @param name Name of the property.
         * @return True if the property exists, false otherwise.
         */
        [[nodiscard]] bool has_vertex_property(const std::string &name) const {
            return vertices.exists(name);
        }

        /**
         * @brief Creates a new vertex.
         * @return The newly created vertex.
         */
        Vertex new_vertex() {
            vertices.push_back();
            return Vertex(vertices.size() - 1);
        }

        /**
        * @brief Marks a vertex as deleted.
        * @param v Vertex to mark as deleted.
        */
        void mark_deleted(const Vertex &v);

        /**
         * @brief Deletes a vertex.
         * @param v Vertex to delete.
         */
        virtual void delete_vertex(const Vertex &v);

        /**
         * @brief Retrieve the halfedge of a vertex.
         * @param v Vertex to retrieve the halfedge for.
         * @return The halfedge of the vertex.
         * */
        Halfedge get_halfedge(const Vertex &v) const {
            return v_connectivity[v].h;
        }

        /**
         * @brief Set the halfedge of a vertex.
         * @param v Vertex to set the halfedge for.
         * @param h Halfedge to set.
         * */
        void set_halfedge(const Vertex &v, const Halfedge &h) {
            v_connectivity[v].h = h;
        }

        /**
         * @brief Retrieve the valence of a vertex.
         * @param v Vertex to retrieve the valence for.
         * @return The valence of the vertex.
         * */
        [[nodiscard]] size_t get_valence(const Vertex &v) const;

        /**
         * @brief Retrieve the indegree of a vertex. Counts the number of incoming edges.
         * @param v Vertex to retrieve the indegree for.
         * @return The indegree of the vertex.
         * @details Undirected edges are counted.
         * */
        [[nodiscard]] size_t get_indegree(const Vertex &v) const;

        /**
         * @brief Retrieve the outdegree of a vertex. Counts the number of outgoing edges.
         * @param v Vertex to retrieve the outdegree for.
         * @return The outdegree of the vertex.
         * @details Undirected edges are counted.
         * */
        [[nodiscard]] size_t get_outdegree(const Vertex &v) const;

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
         * @brief Retrieve the number of halfedges.
         * @return Number of halfedges.
         * */
        [[nodiscard]] size_t n_halfedges() const { return halfedges.n_halfedges(); }

        /**
         * @brief Check if a halfedge is deleted.
         * @param h Halfedge to check.
         * @return True if deleted, false otherwise.
         * */
        [[nodiscard]] bool is_deleted(const Halfedge &h) const { return halfedges.is_deleted(h); }

        /**
         * @brief Check if a halfedge is valid.
         * @param h Halfedge to check.
         * @return True if valid, false otherwise.
         * */
        [[nodiscard]] bool is_valid(const Halfedge &h) const { return halfedges.is_valid(h); }

        /**
         * @brief Mark a halfedge as deleted.
         * @param h Halfedge to mark as deleted.
         * */
        void mark_deleted(const Halfedge &h);

        /**
         * @brief Find a halfedge between two vertices.
         * @param v0 First vertex.
         * @param v1 Second vertex.
         * @return The halfedge between the two vertices.
         * */
        Halfedge find_halfedge(const Vertex &v0, const Vertex &v1) const;

        /**
         * @brief Find an edge between two vertices.
         * @param v0 First vertex.
         * @param v1 Second vertex.
         * @return The edge between the two vertices.
         * */
        Edge find_edge(const Vertex &v0, const Vertex &v1) const;


        /**
         * @brief Sets the target vertex of a halfedge.
         * @param h Halfedge to set the vertex for.
         * @param v The target vertex to set.
         */
        void set_vertex(const Halfedge &h, const Vertex &v) {
            h_connectivity[h].v = v;
        }

        /**
         * @brief Retrieves the target vertex of a halfedge.
         * @param h Halfedge to retrieve the vertex for.
         * @return The target vertex of the halfedge.
         */
        Vertex get_vertex(const Halfedge &h) const {
            return h_connectivity[h].v;
        }

        /**
         * @brief Retrieves the next halfedge of a halfedge.
         * @param h Halfedge to retrieve the next halfedge for.
         * @return The next halfedge of the halfedge.
         */
        Halfedge get_next(const Halfedge &h) const {
            return h_connectivity[h].nh;
        }

        /**
         * @brief Sets the next halfedge of a halfedge.
         * @param h Halfedge to set the next halfedge for.
         * @param nh Next halfedge to set.
         */
        void set_next(const Halfedge &h, const Halfedge &nh) {
            h_connectivity[h].nh = nh;
            h_connectivity[nh].ph = h;
        }

        /**
         * @brief Sets the previous halfedge of a halfedge.
         * @param h Halfedge to set the previous halfedge for.
         * @param ph Previous halfedge to set.
         */
        void set_prev(const Halfedge &h, const Halfedge &ph) {
            h_connectivity[h].ph = ph;
            h_connectivity[ph].nh = h;
        }

        /**
         * @brief Retrieves the previous halfedge of a halfedge.
         * @param h Halfedge to retrieve the previous halfedge for.
         * @return The previous halfedge of the halfedge.
         */
        Halfedge get_prev(const Halfedge &h) const {
            return h_connectivity[h].ph;
        }

        /**
         * @brief Retrieves the opposite halfedge of a halfedge.
         * @param h Halfedge to retrieve the opposite halfedge for.
         * @return The opposite halfedge of the halfedge.
         */
        Halfedge get_opposite(const Halfedge &h) const {
            return Halfedge((h.idx() & 1) ? h.idx() - 1 : h.idx() + 1);
        }

        /**
         * @brief Rotates a halfedge clockwise.
         * @param h Halfedge to rotate.
         * @return The rotated halfedge.
         */
        Halfedge rotate_cw(const Halfedge &h) const {
            return get_next(get_opposite(h));
        }

        /**
         * @brief Rotates a halfedge counterclockwise.
         * @param h Halfedge to rotate.
         * @return The rotated halfedge.
         */
        Halfedge rotate_ccw(const Halfedge &h) const {
            return get_opposite(get_prev(h));
        }

        /**
         * @brief Retrieves the edge of a halfedge.
         * @param h Halfedge to retrieve the edge for.
         * @return The edge of the halfedge.
         */
        Edge get_edge(const Halfedge &h) const {
            return Edge(h.idx() >> 1);
        }

        /**
         * @brief Adds a halfedge property to the Graph.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @param t Default value of the property.
         * @return The added halfedge property.
         */
        template<class T>
        HalfedgeProperty<T> add_halfedge_property(const std::string &name,
                                                  const T t = T()) {
            return HalfedgeProperty<T>(halfedges.add<T>(name, t));
        }

        /**
         * @brief Retrieves a halfedge property by name.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @return The halfedge property if it exists.
         */
        template<class T>
        HalfedgeProperty<T> get_halfedge_property(const std::string &name) const {
            return HalfedgeProperty<T>(halfedges.get<T>(name));
        }

        /**
         * @brief Retrieves or adds a halfedge property.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @param t Default value of the property.
         * @return The halfedge property.
         */
        template<class T>
        HalfedgeProperty<T> halfedge_property(const std::string &name, const T t = T()) {
            return HalfedgeProperty<T>(halfedges.get_or_add<T>(name, t));
        }

        /**
         * @brief Removes a halfedge property.
         * @tparam T Type of the property.
         * @param p Halfedge property to remove.
         */
        template<class T>
        void remove_halfedge_property(HalfedgeProperty<T> &p) {
            halfedges.remove(p);
        }

        /**
         * @brief Check if a halfedge property exists.
         * @param name Name of the property.
         * @return True if the property exists, false otherwise.
         */
        [[nodiscard]] bool has_halfedge_property(const std::string &name) const {
            return halfedges.exists(name);
        }

        // -------------------------------------------------------------------------------------------------------------
        // Edge Methods
        // -------------------------------------------------------------------------------------------------------------

        /**
         * @brief Retrieve the number of edges.
         * @return Number of edges.
         * */
        [[nodiscard]] size_t n_edges() const { return edges.n_edges(); }

        /**
         * @brief Check if an edge is deleted.
         * @param e Edge to check.
         * @return True if deleted, false otherwise.
         * */
        [[nodiscard]] bool is_deleted(const Edge &e) const { return edges.is_deleted(e); }

        /**
         * @brief Check if an edge is valid.
         * @param e Edge to check.
         * @return True if valid, false otherwise.
         * */
        [[nodiscard]] bool is_valid(const Edge &e) const { return edges.is_valid(e); }

        /**
         * @brief Check if an edge is on the boundary.
         * @param e Edge to check.
         * @return True if on the boundary, false otherwise.
         * */
        [[nodiscard]] bool is_boundary(const Edge &e) const {
            return is_boundary(get_halfedge(e, 0)) || is_boundary(get_halfedge(e, 1));
        }

        /**
         * @brief Create a new edge.
         * @param v0 First vertex of the edge.
         * @param v1 Second vertex of the edge.
         * @return The halfedge to v1.
         * @details Sets only the target vertex of the halfedges of the edge automatically.
         * */
        Halfedge new_edge(const Vertex &v0, const Vertex &v1);

        /**
         * @brief Add an edge between two vertices.
         * @param v0 First vertex of the edge.
         * @param v1 Second vertex of the edge.
         * @return The halfedge to v1.
         * @details Updates the connectivity of the vertices and halfedges automatically.
         * */
        Halfedge add_edge(const Vertex &v0, const Vertex &v1);

        /**
         * @brief Set the direction of an edge.
         * @param e Edge to set the direction for.
         * @param h Halfedge to set as the direction.
         */
        void set_direction(const Edge &e, const Halfedge &h) {
            e_direction[e] = h;
        }

        /**
         * @brief Set the edge as undirected.
         * @param e Edge to set as undirected.
         */
        void set_undirected(const Edge &e) {
            e_direction[e] = Halfedge();
        }

        /**
         * @brief Retrieve if an edge is directed.
         * @param e Edge to check.
         * @return True if directed, false otherwise.
         */
        [[nodiscard]] bool is_directed(const Edge &e) const {
            return e_direction[e].is_valid();
        }

        /**
         *  @brief Splits an edge by inserting a vertex.
         *  @param e Edge to split.
         *  @param v Vertex to insert.
         *  @return The new Halfedge pointing to the inserted vertex from the target vertex of the edge.
         */
        virtual Halfedge split(const Edge &e, const Vertex &v);

        /**
         * @brief Marks an edge as deleted.
         * @param e Edge to mark as deleted.
         */
        void mark_deleted(const Edge &e);

        /**
         * @brief Deletes an edge and all incident faces.
         * @param e Edge to delete.
         */
        virtual void delete_edge(const Edge &e);

        /**
         * @brief Retrieves the halfedge of an edge.
         * @param e Edge to retrieve the halfedge for.
         * @param i Index of the halfedge.
         * @return The halfedge of the edge.
         */
        Halfedge get_halfedge(const Edge &e, int i) const {
            return Halfedge{(e.idx() << 1) + i};
        }

        /**
         * @brief Retrieves the vertex of an edge.
         * @param e Edge to retrieve the vertex for.
         * @param i Index of the vertex.
         * @return The vertex of the edge.
         */
        Vertex get_vertex(const Edge &e, int i) const {
            return get_vertex(get_halfedge(e, i));
        }

        /**
         * @brief Adds an edge property to the Mesh.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @param t Default value of the property.
         * @return The added edge property.
         */
        template<class T>
        EdgeProperty<T> add_edge_property(const std::string &name,
                                          const T t = T()) {
            return EdgeProperty<T>(edges.add<T>(name, t));
        }

        /**
         * @brief Retrieves an edge property by name.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @return The edge property if it exists.
         */
        template<class T>
        EdgeProperty<T> get_edge_property(const std::string &name) const {
            return EdgeProperty<T>(edges.get<T>(name));
        }

        /**
         * @brief Retrieves or adds an edge property.
         * @tparam T Type of the property.
         * @param name Name of the property.
         * @param t Default value of the property.
         * @return The edge property.
         */
        template<class T>
        EdgeProperty<T> edge_property(const std::string &name, const T t = T()) {
            return EdgeProperty<T>(edges.get_or_add<T>(name, t));
        }

        /**
         * @brief Removes an edge property.
         * @tparam T Type of the property.
         * @param p Edge property to remove.
         */
        template<class T>
        void remove_edge_property(EdgeProperty<T> &p) {
            edges.remove(p);
        }

        /**
         * @brief Check if an edge property exists.
         * @param name Name of the property.
         * @return True if the property exists, false otherwise.
         */
        [[nodiscard]] bool has_edge_property(const std::string &name) const {
            return edges.exists(name);
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
        [[nodiscard]] bool is_isolated(Vertex v) const { return !get_halfedge(v).is_valid(); }

        /**
         * @brief Checks if a vertex is on the boundary. The vertex is on the boundary if its outgoing halfedge is a boundary halfedge. It is adjusted like this during insertion and deletion.
         * @param v Vertex to check.
         * @return True if the vertex is on the boundary, false otherwise.
         */
        [[nodiscard]] bool is_boundary(const Vertex &v) const {
            Halfedge h = get_halfedge(v);
            return !(h.is_valid() && get_face(h).is_valid());
        }


        /**
         * @brief Adjusts the outgoing halfedge of a vertex to be on the boundary.
         * @param v Vertex to adjust the outgoing halfedge for.
         */
        void adjust_outgoing_halfedge(const Vertex &v);


        // -------------------------------------------------------------------------------------------------------------
        // Halfedge Methods
        // -------------------------------------------------------------------------------------------------------------

        /**
         * @brief Checks if a halfedge is on the boundary. A halfedge is on the boundary if its face is invalid.
         * @param h Halfedge to check.
         * @return True if the halfedge is on the boundary, false otherwise.
         */
        [[nodiscard]] bool is_boundary(const Halfedge &h) const { return !get_face(h).is_valid(); }

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
         * @param v Vertex to insert.
         * @return The new halfedge pointing to the inserted vertex from the target vertex of the edge.
         */
        Halfedge insert_vertex(const Edge &e, const Vertex &v) {
            return insert_vertex(get_halfedge(e, 0), v);
        }


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

    /**
     * @brief Inserts a vertex along an edge in the mesh.
     *
     * This function adds a new vertex at the specified position along the given edge
     * and updates the mesh topology accordingly.
     *
     * @tparam T The data type of the vertex positions.
     * @tparam N The dimension of the vertex positions.
     * @param mesh The mesh to modify.
     * @param positions The vertex property containing the positions of the vertices.
     * @param e The edge along which to insert the new vertex.
     * @param p The position of the new vertex.
     * @return The halfedge pointing to the new vertex.
     */
    template<typename T, int N>
    Halfedge insert_vertex(Mesh &mesh, VertexProperty<Vector<T, N>> &positions, const Edge &e, const Vector<Real, 3> &p) {
        return mesh.insert_vertex(e, add_vertex(mesh.vertices, positions, p));
    }

    /**
     * @brief Splits a face by inserting a vertex at the specified position.
     *
     * This function adds a new vertex at the given position within the specified face
     * and updates the mesh topology accordingly.
     *
     * @tparam T The data type of the vertex positions.
     * @tparam N The dimension of the vertex positions.
     * @param mesh The mesh to modify.
     * @param positions The vertex property containing the positions of the vertices.
     * @param f The face to split.
     * @param p The position of the new vertex.
     * @return The newly created vertex.
     */
    template<typename T, int N>
    Vertex split(Mesh &mesh, VertexProperty<Vector<T, N>> &positions, Face f, const Vector<Real, 3> &p) {
        Vertex v = add_vertex(mesh.vertices, positions, p);
        mesh.split(f, v);
        return v;
    }
}

#endif //MESH_H
