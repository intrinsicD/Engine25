//
// Created by alex on 19.02.25.
//

#ifndef ENGINE25_VERTICES_H
#define ENGINE25_VERTICES_H

#include "GeometricProperties.h"

namespace Bcg{
    /**
     * @class Vertices
     * @brief A class for managing 3D point clouds, handling vertices and associated properties.
     */
    class Vertices {
    public:
        VertexProperty<bool> v_deleted; ///< Deletion flag for vertices.
        VertexContainer vertices; ///< Container for managing vertex data.

        /**
         * @brief Default constructor. Initializes properties and containers.
         */
        Vertices();

        /**
         * @brief Virtual destructor for extensibility.
         */
        virtual ~Vertices() = default;

        /**
         * @brief Copy constructor.
         * @param rhs Another Vertices instance to copy.
         */
        Vertices(const Vertices &rhs) { operator=(rhs); }

        /**
         * @brief Copy assignment operator.
         * @param rhs Another Vertices instance to copy.
         * @return Reference to this Vertices.
         */
        Vertices &operator=(const Vertices &rhs);

        /**
         * @brief Deep copy assignment method.
         * @param rhs Another Vertices instance to copy.
         * @return Reference to this Vertices.
         */
        Vertices &assign(const Vertices &rhs);

        /**
         * @brief Clears all vertices and properties.
         */
        virtual void clear();

        /**
         * @brief Releases unused memory.
         */
        virtual void free_memory();

        /**
         * @brief Reserves space for a specified number of vertices.
         * @param nvertices Number of vertices to reserve space for.
         */
        virtual void reserve(size_t nvertices);

        /**
         * @brief Removes deleted vertices and compacts the data.
         */
        virtual void garbage_collection();

        // -------------------------------------------------------------------------------------------------------------
        // Vertex Methods
        // -------------------------------------------------------------------------------------------------------------

        /**
         * @brief Returns the total number of undeleted vertices.
         * @return Number of undeleted vertices.
         */
        [[nodiscard]] size_t n_vertices() const { return vertices.n_vertices(); }

        /**
         * @brief Checks if the Vertices is empty.
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
         * @brief Checks if there is garbage (deleted vertices).
         * @return True if garbage exists, false otherwise.
         */
        [[nodiscard]] virtual bool has_garbage() const { return vertices.has_garbage(); }

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
    };
}

#endif //ENGINE25_VERTICES_H
