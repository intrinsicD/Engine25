//
// Created by alex on 08.01.25.
//

#include "PointCloud.h"

namespace Bcg {
    PointCloud::PointCloud() : vertices() {
        // link properties to containers
        positions = vertices.vertex_property<Vector<Real, 3>>("v:position");
        v_deleted = vertices.vertex_property<bool>("v:deleted", false);
    }

    PointCloud &PointCloud::operator=(const PointCloud &rhs) {
        if (this != &rhs) {
            // deep copy of property containers
            vertices = rhs.vertices;

            // link properties from copied containers
            positions = vertices.vertex_property<Vector<Real, 3>>("v:position");
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

            // copy properties from other mesh
            positions.vector() = rhs.positions.vector();

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
        positions = vertices.vertex_property<Vector<Real, 3>>("v:position", Vector<Real, 3>::Zero());
    }

    void PointCloud::free_memory() {
        vertices.free_memory();
    }

    void PointCloud::reserve(size_t nvertices) {
        vertices.reserve(nvertices);
    };

    Vertex PointCloud::add_vertex(const Vector<Real, 3> &p) {
        Vertex v = vertices.new_vertex();
        if (v.is_valid()) {
            positions[v] = p;
        }
        return v;
    }

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

        // setup handle mapping
        VertexProperty<Vertex> vmap = vertices.vertex_property<Vertex>("v:garbage_collection");

        for (size_t i = 0; i < nV; ++i)
            vmap[Vertex(i)] = Vertex(i);

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

        // remove handle maps
        vertices.remove_vertex_property(vmap);

        // finally resize arrays
        vertices.resize(nV);
        free_memory();

        vertices.num_deleted = 0;
    };
}