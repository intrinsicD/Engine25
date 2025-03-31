//
// Created by alex on 08.01.25.
//

#ifndef ENGINE25_GRAPH_H
#define ENGINE25_GRAPH_H

#include "PointCloud.h"
#include <stack>
#include <queue>

namespace Bcg {
    //---------------------------------------------------------------------
    // DFS Iterator for Graph (vertex traversal)
    //---------------------------------------------------------------------
    template<class GraphType>
    class GraphDFSIterator {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Vertex;
        using reference = const Vertex &;
        using pointer = const Vertex *;
        using iterator_category = std::forward_iterator_tag;

        // Constructor: initialize with a starting vertex.
        GraphDFSIterator(const GraphType *graph, const Vertex &start)
                : graph_(graph), visited_(graph ? graph->n_vertices() : 0, false) {
            if (graph_ && graph_->is_valid(start)) {
                stack_.push(start);
                visited_[start.idx()] = true;
            }
        }

        // Default constructor for the end iterator.
        GraphDFSIterator() : graph_(nullptr) {
        }

        reference operator*() const {
            return stack_.top();
        }

        pointer operator->() const {
            return &stack_.top();
        }

        GraphDFSIterator &operator++() {
            if (stack_.empty()) {
                graph_ = nullptr;
                return *this;
            }

            // Pop the current vertex.
            Vertex current = stack_.top();
            stack_.pop();

            // For each neighbor of current (using your get_vertices circulator),
            // push unvisited neighbors onto the stack.
            for (const Vertex &nbr: graph_->get_vertices(current)) {
                if (!visited_[nbr.idx()]) {
                    visited_[nbr.idx()] = true;
                    stack_.push(nbr);
                }
            }

            if (stack_.empty()) {
                graph_ = nullptr; // Mark end.
            }
            return *this;
        }

        GraphDFSIterator operator++(int) {
            GraphDFSIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const GraphDFSIterator &other) const {
            // Both end iterators have graph_ set to nullptr.
            if (graph_ == nullptr && other.graph_ == nullptr) {
                return true;
            }
            // Compare the graph pointer and the top of the stack.
            return graph_ == other.graph_ && stack_.top() == other.stack_.top();
        }

        bool operator!=(const GraphDFSIterator &other) const {
            return !(*this == other);
        }

    private:
        const GraphType *graph_;
        std::stack<Vertex, std::vector<Vertex> > stack_;
        std::vector<bool> visited_;
    };

    //---------------------------------------------------------------------
    // BFS Iterator for Graph (vertex traversal)
    //---------------------------------------------------------------------
    template<class GraphType>
    class GraphBFSIterator {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Vertex;
        using reference = const Vertex &;
        using pointer = const Vertex *;
        using iterator_category = std::forward_iterator_tag;

        // Constructor: initialize with a starting vertex.
        GraphBFSIterator(const GraphType *graph, const Vertex &start)
                : graph_(graph), visited_(graph ? graph->n_vertices() : 0, false) {
            if (graph_ && graph_->is_valid(start)) {
                queue_.push(start);
                visited_[start.idx()] = true;
            }
        }

        // Default constructor for the end iterator.
        GraphBFSIterator() : graph_(nullptr) {
        }

        reference operator*() const {
            return queue_.front();
        }

        pointer operator->() const {
            return &queue_.front();
        }

        GraphBFSIterator &operator++() {
            if (queue_.empty()) {
                graph_ = nullptr;
                return *this;
            }
            Vertex current = queue_.front();
            queue_.pop();

            // For each neighbor of current, push unvisited neighbors onto the queue.
            for (const Vertex &nbr: graph_->get_vertices(current)) {
                if (!visited_[nbr.idx()]) {
                    visited_[nbr.idx()] = true;
                    queue_.push(nbr);
                }
            }
            if (queue_.empty()) {
                graph_ = nullptr;
            }
            return *this;
        }

        GraphBFSIterator operator++(int) {
            GraphBFSIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const GraphBFSIterator &other) const {
            if (graph_ == nullptr && other.graph_ == nullptr)
                return true;
            return graph_ == other.graph_ &&
                   (!queue_.empty() && !other.queue_.empty() &&
                    queue_.front() == other.queue_.front());
        }

        bool operator!=(const GraphBFSIterator &other) const {
            return !(*this == other);
        }

    private:
        const GraphType *graph_;
        std::queue<Vertex> queue_;
        std::vector<bool> visited_;
    };

    //---------------------------------------------------------------------
    // Range wrappers for Graph DFS and BFS traversals
    //---------------------------------------------------------------------
    template<class GraphType>
    class GraphDFSRange {
    public:
        GraphDFSRange(const GraphType *graph, const Vertex &start)
                : graph_(graph), start_(start) {
        }

        GraphDFSIterator<GraphType> begin() const {
            return GraphDFSIterator<GraphType>(graph_, start_);
        }

        GraphDFSIterator<GraphType> end() const {
            return GraphDFSIterator<GraphType>();
        }

    private:
        const GraphType *graph_;
        Vertex start_;
    };

    template<class GraphType>
    class GraphBFSRange {
    public:
        GraphBFSRange(const GraphType *graph, const Vertex &start)
                : graph_(graph), start_(start) {
        }

        GraphBFSIterator<GraphType> begin() const {
            return GraphBFSIterator<GraphType>(graph_, start_);
        }

        GraphBFSIterator<GraphType> end() const {
            return GraphBFSIterator<GraphType>();
        }

    private:
        const GraphType *graph_;
        Vertex start_;
    };

    /**
     * @brief A Graph class representing a halfedge-based data structure, inheriting from PointCloud.
     * This class provides mechanisms to store, manipulate, and traverse vertices, edges, and halfedges.
     */
    class Graph {
    public:
        using VertexAroundVertexCirculator = VertexAroundVertexCirculatorBase<Graph>;
        using HalfedgeAroundVertexCirculator = HalfedgeAroundVertexCirculatorBase<Graph>;
        using EdgeAroundVertexCirculator = EdgeAroundVertexCirculatorBase<Graph>;

        VertexContainer vertices;       ///< Container for managing vertex data.
        HalfedgeContainer halfedges;    ///< Container for managing halfedge data.
        EdgeContainer edges;            ///< Container for managing edge data.

        VertexProperty<bool> v_deleted; ///< Deletion flag for vertices.
        VertexProperty<VertexConnectivity> v_connectivity; ///< Connectivity information for vertices.

        HalfedgeProperty<bool> h_deleted; ///< Deletion flag for halfedges.
        HalfedgeProperty<HalfedgeConnectivity> h_connectivity; ///< Connectivity information for halfedges.

        EdgeProperty<bool> e_deleted;   ///< Deletion flag for edges.
        EdgeProperty<Halfedge> e_direction; ///< Direction of the edge (halfedge) in the graph.

        /**
         * @brief Default constructor for Graph, initializes vertex, edge, and halfedge properties.
         * */
        Graph();

        Graph(VertexContainer vertices, HalfedgeContainer halfedges, EdgeContainer edges);

        /**
         * @brief Virtual destructor for extensibility.
         * */
        virtual ~Graph() = default;

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
        virtual void clear();

        /**
         * @brief Releases unused memory.
         */
        virtual void free_memory();

        /**
         * @brief Reserve memory for vertices, edges, and faces.
         * @param nvertices Number of vertices to reserve.
         * @param nedges Number of edges to reserve.
         * @param nfaces Number of faces to reserve.
         * @param ntets Number of tets to reserve.
         * */
        virtual void reserve(size_t nvertices, size_t nedges);

        /**
         * @brief Removes deleted vertices, edges, and faces and compacts the data.
         * */
        virtual void garbage_collection();

        /**
         * @brief Check if the Graph has any deleted vertices, edges, or faces.
         * @return True if the Graph has any deleted elements, false otherwise.
         * */
        [[nodiscard]] bool has_garbage() const {
            return vertices.has_garbage() ||
                   halfedges.has_garbage() ||
                   edges.has_garbage();
        }

        // -------------------------------------------------------------------------------------------------------------
        // Vertex Methods
        // -------------------------------------------------------------------------------------------------------------

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
         * @brief Returns a DFS range for traversing the graph starting at a given vertex.
         * @param start The starting vertex.
         * @return A range that can be used in a range-based for loop.
         */
        GraphDFSRange<Graph> dfs(const Vertex &start) const {
            return GraphDFSRange<Graph>(this, start);
        }

        /**
         * @brief Returns a BFS range for traversing the graph starting at a given vertex.
         * @param start The starting vertex.
         * @return A range that can be used in a range-based for loop.
         */
        GraphBFSRange<Graph> bfs(const Vertex &start) const {
            return GraphBFSRange<Graph>(this, start);
        }

        //TODO: provide dfs and bfs iterators and range based for loops similar to tree class
    };

    /**
     *  @brief Splits an edge by inserting a vertex.
     *  @param e Edge to split.
     *  @param p Position of the vertex to insert.
     *  @return The new Halfedge pointing to the inserted vertex from the target vertex of the edge.
     */
    template<typename T, int N>
    Halfedge split(Graph &graph, VertexProperty<Vector<T, N>> &positions, const Edge &e, const Vector<T, N> &p) {
        return graph.split(e, add_vertex(graph.vertices, positions, p));
    }
}

#endif //ENGINE25_GRAPH_H
