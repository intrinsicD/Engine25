//
// Created by alex on 19.02.25.
//

#include "Vertices.h"

namespace Bcg {
    Vertices::Vertices() : vertices() {
        // link properties to containers
        v_deleted = vertices.vertex_property<bool>("v:deleted", false);
    }

    Vertices &Vertices::operator=(const Vertices &rhs) {
        if (this != &rhs) {
            // deep copy of property containers
            vertices = rhs.vertices;

            // link properties from copied containers
            v_deleted = vertices.vertex_property<bool>("v:deleted");
            vertices.num_deleted = rhs.vertices.num_deleted;
        }

        return *this;
    }

    Vertices &Vertices::assign(const Vertices &rhs) {
        if (this != &rhs) {
            // clear properties
            clear();

            // resize (needed by property containers)
            vertices.resize(rhs.vertices.size());

            // copy properties from other mesh

            v_deleted.vector() = rhs.v_deleted.vector();

            // how many elements are deleted?
            vertices.num_deleted = rhs.vertices.num_deleted;
        }

        return *this;
    }

    void Vertices::clear() {
        vertices.clear();
        free_memory();

        v_deleted = vertices.vertex_property<bool>("v:deleted", false);
    }

    void Vertices::free_memory() {
        vertices.free_memory();
    }

    void Vertices::reserve(size_t nvertices) {
        vertices.reserve(nvertices);
    };

    void Vertices::mark_deleted(const Vertex &v) {
        if (v_deleted[v])
            return;

        v_deleted[v] = true;
        ++vertices.num_deleted;
    }

    void Vertices::delete_vertex(const Vertex &v) {
        mark_deleted(v);
    }

    void Vertices::garbage_collection() {
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
    };
}