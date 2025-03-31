//
// Created by alex on 08.01.25.
//

#ifndef ENGINE25_POINTCLOUD_H
#define ENGINE25_POINTCLOUD_H

#include "GeometricProperties.h"

namespace Bcg {
    /**
     * @class PointCloud
     * @brief A class for managing and handling vertices and associated properties.
     */
    class PointCloud {
    public:
        VertexContainer vertices; ///< Container for managing vertex data.

        VertexProperty<bool> v_deleted; ///< Deletion flag for vertices.

        /**
         * @brief Default constructor. Initializes properties and containers.
         */
        PointCloud();

        /**
         * @brief Constructs a PointCloud object with the given vertices.
         * @param vertices The container of vertices to initialize the PointCloud with.
         */
        explicit PointCloud(VertexContainer vertices);

        /**
         * @brief Virtual destructor for extensibility.
         */
        virtual ~PointCloud() = default;

        /**
         * @brief Copy constructor.
         * @param rhs Another PointCloud instance to copy.
         */
        PointCloud(const PointCloud &rhs) { operator=(rhs); }

        /**
         * @brief Copy assignment operator.
         * @param rhs Another PointCloud instance to copy.
         * @return Reference to this PointCloud.
         */
        PointCloud &operator=(const PointCloud &rhs);

        /**
         * @brief Deep copy assignment method.
         * @param rhs Another PointCloud instance to copy.
         * @return Reference to this PointCloud.
         */
        PointCloud &assign(const PointCloud &rhs);

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

    template<typename T, int N>
    Vertex add_vertex(VertexContainer &vertices, VertexProperty<Vector<T, N>> &positions, const Vector<T, N> &p) {
        vertices.push_back();
        auto v = Vertex(vertices.size() - 1);;
        if (v.is_valid()) {
            positions[v] = p;
        }
        return v;
    }
}

#endif //ENGINE25_POINTCLOUD_H
