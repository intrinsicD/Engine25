//
// Created by alex on 08.01.25.
//

#include "PointCloud.h"

#include <utility>

namespace Bcg {
    PointCloud::PointCloud() {
        // link properties to containers
        v_deleted = vertices.vertex_property<bool>("v:deleted", false);

        assert(v_deleted);
    }

    PointCloud::PointCloud(VertexContainer vertices) : vertices(std::move(vertices)) {
        // link properties to containers
        v_deleted = vertices.vertex_property<bool>("v:deleted", false);

        assert(v_deleted);
    }

    PointCloud &PointCloud::operator=(const PointCloud &rhs) {
        if (this != &rhs) {
            // deep copy of property containers
            vertices = rhs.vertices;

            // link properties from copied containers
            v_deleted = vertices.vertex_property<bool>("v:deleted");
            vertices.num_deleted = rhs.vertices.num_deleted;
        }

        return *this;
    }

    PointCloud &PointCloud::assign(const PointCloud &rhs) {
        if (this != &rhs) {
            // clear properties
            clear();

            // resize (needed by property containers)
            vertices.resize(rhs.vertices.size());

            if (rhs.has_vertex_property("v:position")) {
                // copy properties from other mesh
                vertices.link("v:position", rhs.vertices.get_base("v:position")->clone());
            }

            // copy properties from other mesh
            v_deleted.vector() = rhs.v_deleted.vector();

            // how many elements are deleted?
            vertices.num_deleted = rhs.vertices.num_deleted;
        }

        return *this;
    }

    void PointCloud::clear() {
        vertices.clear();
        free_memory();

        v_deleted = vertices.vertex_property<bool>("v:deleted", false);
    }


    void PointCloud::free_memory() {
        vertices.free_memory();
    }

    void PointCloud::reserve(size_t nvertices) {
        vertices.reserve(nvertices);
    };

    void PointCloud::mark_deleted(const Vertex &v) {
        if (v_deleted[v])
            return;

        v_deleted[v] = true;
        ++vertices.num_deleted;
    }

    void PointCloud::delete_vertex(const Vertex &v) {
        mark_deleted(v);
    }

    void PointCloud::garbage_collection() {
        if (!vertices.has_garbage())
            return;

        auto nV = vertices.size();

        // remove deleted vertices
        if (nV > 0) {
            size_t i0 = 0;
            size_t i1 = nV - 1;

            while (true) {
                // find first deleted and last un-deleted
                while (!v_deleted[Vertex(i0)] && i0 < i1)
                    ++i0;
                while (v_deleted[Vertex(i1)] && i0 < i1)
                    --i1;
                if (i0 >= i1)
                    break;

                // swap
                vertices.swap(i0, i1);
            }

            // remember new size
            nV = v_deleted[Vertex(i0)] ? i0 : i0 + 1;
        }

        // finally resize arrays
        vertices.resize(nV);
        free_memory();

        vertices.num_deleted = 0;
    }
}