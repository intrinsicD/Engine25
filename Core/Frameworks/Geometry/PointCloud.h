//
// Created by alex on 08.01.25.
//

#ifndef ENGINE25_POINTCLOUD_H
#define ENGINE25_POINTCLOUD_H

#include "GeometricProperties.h"

namespace Bcg {
    class PointCloud {
    public:
        VertexProperty<Vector<Real, 3>> positions;
        VertexProperty<bool> v_deleted;
        Vertices vertices;

        PointCloud();

        virtual ~PointCloud() = default;

        PointCloud(const PointCloud &rhs) { operator=(rhs); }

        PointCloud &operator=(const PointCloud &rhs);

        PointCloud &assign(const PointCloud &rhs);

        void clear();

        void free_memory();

        void reserve(size_t nvertices);

        void garbage_collection();

        [[nodiscard]] inline size_t n_vertices() const { return vertices.n_vertices(); }

        [[nodiscard]] inline bool is_empty() const { return n_vertices() == 0; }

        [[nodiscard]] inline bool is_deleted(const Vertex &v) const { return vertices.is_deleted(v); }

        [[nodiscard]] inline bool is_valid(const Vertex &v) const { return vertices.is_valid(v); }

        [[nodiscard]] inline bool has_garbage() const { return vertices.has_garbage(); }

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

        inline Vertex new_vertex() { return vertices.new_vertex(); }

        Vertex add_vertex(const Vector<Real, 3> &p);

        void mark_deleted(const Vertex &v);

        void delete_vertex(const Vertex &v);
    };
}

#endif //ENGINE25_POINTCLOUD_H
