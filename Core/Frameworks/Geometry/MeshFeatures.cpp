//
// Created by alex on 2/10/25.
//

#include "MeshFeatures.h"
#include "MeshUtils.h"

namespace Bcg {
    size_t DetectFeatures(Mesh &mesh, Real angle) {
        auto vfeature = mesh.vertex_property("v:feature", false);
        auto efeature = mesh.edge_property("e:feature", false);
        const Real feature_cosine = cos(angle / 180.0 * std::numbers::pi);
        size_t n_edges = 0;
        auto positions = mesh.vertex_property<Vector<Real, 3> >("v:position");
        for (auto e: mesh.edges) {
            if (!mesh.is_boundary(e)) {
                const auto f0 = mesh.get_face(mesh.get_halfedge(e, 0));
                const auto f1 = mesh.get_face(mesh.get_halfedge(e, 1));

                const Vector<Real, 3> n0 = FaceNormal(mesh, positions, f0);
                const Vector<Real, 3> n1 = FaceNormal(mesh, positions, f1);

                if (n0.dot(n1) < feature_cosine) {
                    efeature[e] = true;
                    vfeature[mesh.get_vertex(e, 0)] = true;
                    vfeature[mesh.get_vertex(e, 1)] = true;
                    n_edges++;
                }
            }
        }
        return n_edges;
    }

    size_t DetectBoundary(Mesh &mesh) {
        auto vfeature = mesh.vertex_property("v:feature", false);
        auto efeature = mesh.edge_property("e:feature", false);
        for (auto v: mesh.vertices)
            if (mesh.is_boundary(v))
                vfeature[v] = true;

        size_t n_edges = 0;
        for (auto e: mesh.edges)
            if (mesh.is_boundary(e)) {
                efeature[e] = true;
                n_edges++;
            }
        return n_edges;
    }

    void ClearFeatures(Mesh &mesh) {
        auto vfeature = mesh.vertex_property("v:feature", false);
        auto efeature = mesh.edge_property("e:feature", false);
        for (auto v: mesh.vertices)
            vfeature[v] = false;

        for (auto e: mesh.edges)
            efeature[e] = false;
    }
}
