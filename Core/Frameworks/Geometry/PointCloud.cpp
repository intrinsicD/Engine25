//
// Created by alex on 08.01.25.
//

#include "PointCloud.h"

namespace Bcg {
    PointCloud::PointCloud() : Vertices() {
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

    Vertex PointCloud::add_vertex(const Vector<Real, 3> &p) {
        Vertex v = new_vertex();
        if (v.is_valid()) {
            positions[v] = p;
        }
        return v;
    }
}