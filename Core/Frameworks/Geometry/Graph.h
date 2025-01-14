//
// Created by alex on 08.01.25.
//

#ifndef ENGINE25_GRAPH_H
#define ENGINE25_GRAPH_H

#include "PointCloud.h"

namespace Bcg {
    /**
     * @brief A Graph class representing a halfedge-based data structure, inheriting from PointCloud.
     * This class provides mechanisms to store, manipulate, and traverse vertices, edges, and halfedges.
     */
    class Graph : public PointCloud {
    public:
        using VertexAroundVertexCirculator = VertexAroundVertexCirculatorBase<Graph>;
        using HalfedgeAroundVertexCirculator = HalfedgeAroundVertexCirculatorBase<Graph>;
        using EdgeAroundVertexCirculator = EdgeAroundVertexCirculatorBase<Graph>;

        using PointCloud::mark_deleted;

        Halfedges halfedges;
        Edges edges;

        VertexProperty<VertexConnectivity> v_connectivity;

        HalfedgeProperty<bool> h_deleted;
        HalfedgeProperty<HalfedgeConnectivity> h_connectivity;

        EdgeProperty<bool> e_deleted;
        EdgeProperty<Halfedge> e_direction;

        /**
         * @brief Default constructor for Graph, initializes vertex, edge, and halfedge properties.
         * */
        Graph();

        /**
         * @brief Virtual destructor for extensibility.
         * */
        ~Graph() override = default;

        /**
         * @brief Copy constructor.
         * @param rhs Another Graph instance to copy.
         * */
        Graph(const Graph &rhs) { operator=(rhs); }

        /**
         * @brief Copy assignment operator.
         * @param rhs Another Graph instance to copy.
         * @return Reference to this Graph.
         * */
        Graph &operator=(const Graph &rhs);

        /**
         * @brief Deep copy assignment method.
         * @param rhs Another Graph instance to copy.
         * @return Reference to this Graph.
         * */
        Graph &assign(const Graph &rhs);

        /**
         * @brief Clear all properties and free memory.
         * */
        void clear() override;

        /**
         * @brief Releases unused memory.
         */
        void free_memory() override;

        /**
         * @brief Reserve memory for vertices, edges, and faces.
         * @param nvertices Number of vertices to reserve.
         * @param nedges Number of edges to reserve.
         * @param nfaces Number of faces to reserve.
         * @param ntets Number of tets to reserve.
         * */
        void reserve(size_t nvertices, size_t nedges);

        /**
         * @brief Removes deleted vertices, edges, and faces and compacts the data.
         * */
        void garbage_collection() override;

        /**
         * @brief Check if the Graph has any deleted vertices, edges, or faces.
         * @return True if the Graph has any deleted elements, false otherwise.
         * */
        [[nodiscard]] bool has_garbage() const override {
            return vertices.has_garbage() ||
                   halfedges.has_garbage() ||
                   edges.has_garbage();
        }

        // -------------------------------------------------------------------------------------------------------------
        // Vertex Methods
        // -------------------------------------------------------------------------------------------------------------

        /**
         * @brief Check if a vertex is isolated.
         * @param v Vertex to check.
         * @return True if isolated, false otherwise.
         * */
        [[nodiscard]] bool is_isolated(const Vertex &v) const { return !is_valid(get_halfedge(v)); }

        /**
         * @brief Check if a vertex is on the boundary.
         * @param v Vertex to check.
         * @return True if on the boundary, false otherwise.
         * */
        [[nodiscard]] virtual bool is_boundary(const Vertex &v) const {
            return is_boundary(get_opposite(get_halfedge(v)));
        }

        /**
         * @brief Delete a vertex.
         * @param v Vertex to delete.
         * */
        void delete_vertex(const Vertex &v) override;

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
         * @brief Check if a halfedge is on the boundary.
         * @param h Halfedge to check.
         * @return True if on the boundary, false otherwise.
         * */
        [[nodiscard]] virtual bool is_boundary(const Halfedge &h) const { return get_next(h) == get_opposite(h); }

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
         *  @param p Position of the vertex to insert.
         *  @return The new Halfedge pointing to the inserted vertex from the target vertex of the edge.
         */
        Halfedge split(const Edge &e, const Vector<Real, 3> &p) { return split(e, add_vertex(p)); }

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
         * @brief Perform a depth-first search traversal of the Graph.
         * @param start Vertex to start the traversal from.
         * @param vertex_action Function to execute on each vertex.
         * @param halfedge_action Function to execute on each halfedge.
         * @return bool vector of visited vertices.
         * @details early stopping is supported by returning false from the vertex_action or halfedge_action.
         */
        [[nodiscard]] std::vector<bool> dfs_general_with_early_stopping(const Vertex &start,
                                                                        std::function<bool(const Vertex &)>
                                                                        vertex_action,
                                                                        std::function<bool(const Halfedge &)>
                                                                        halfedge_action);

        /**
         * @brief Perform a breadth-first search traversal of the Graph.
         * @param start Vertex to start the traversal from.
         * @param vertex_action Function to execute on each vertex.
         * @param halfedge_action Function to execute on each halfedge.
         * @return bool vector of visited vertices.
         * @details early stopping is supported by returning false from the vertex_action or halfedge_action.
         */
        [[nodiscard]] std::vector<bool> bfs_general_with_early_stopping(const Vertex &start,
                                                                        std::function<bool(const Vertex &)>
                                                                        vertex_action,
                                                                        std::function<bool(const Halfedge &)>
                                                                        halfedge_action);

        //TODO: Move these to separate files

        EdgeProperty<Vector<unsigned int, 2> > get_edges();

        template<typename T, int N>
        Vector<T, N> vector(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) const {
            return pos[v1] - pos[v0];
        }

        template<typename T, int N>
        Vector<T, N> vector(const Edge &e, const VertexProperty<Vector<T, N> > &pos) const {
            return vector(get_vertex(e, 0), get_vertex(e, 1), pos);
        }

        template<typename T, int N>
        Vector<T, N> vector(const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) const {
            return vector(get_edge(h), pos);
        }

        template<typename T, int N>
        Real length(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) const {
            return vector(v0, v1, pos).norm();
        }

        template<typename T, int N>
        Real length(const Edge &e, const VertexProperty<Vector<T, N> > &pos) const {
            return vector(e, pos).norm();
        }

        template<typename T, int N>
        Real length(const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) const {
            return vector(h, pos).norm();
        }

        template<typename T, int N>
        Vector<T, N> center(const Vertex &v0, const Vertex &v1, const VertexProperty<Vector<T, N> > &pos) const {
            return (pos[v0] + pos[v1]) / 2;
        }

        template<typename T, int N>
        Vector<T, N> center(const Edge &e, const VertexProperty<Vector<T, N> > &pos) const {
            return center(get_vertex(e, 0), get_vertex(e, 1), pos);
        }

        template<typename T, int N>
        Vector<T, N> center(const Halfedge &h, const VertexProperty<Vector<T, N> > &pos) const {
            return center(get_edge(h), pos);
        }
    };
}

#endif //ENGINE25_GRAPH_H
