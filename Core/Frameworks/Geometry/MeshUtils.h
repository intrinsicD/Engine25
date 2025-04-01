//
// Created by alex on 2/5/25.
//

#ifndef MESHUTILS_H
#define MESHUTILS_H

#include "Mesh.h"

namespace Bcg {
    //------------------------------------------------------------------------------------------------------------------
    // Mesh Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] bool ValidateMesh(const Mesh &mesh);

    [[nodiscard]] Mesh Dual(const Mesh &mesh);

    [[nodiscard]] Real VolumeTetrahedralDecomposition(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions);

    [[nodiscard]] Real VolumeDivergenceTheorem(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions);

    [[nodiscard]] Real SurfaceArea(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions);

    [[nodiscard]] Vector<Real, 3> Centroid(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions);

    //------------------------------------------------------------------------------------------------------------------
    // Face Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] Real FaceArea(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f);

    template<typename T, int N>
    Vector<T, N> FaceCenter(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Face &f) {
        Vector<double, N> center = Vector<double, N>::Zero();
        int count = 0;
        for (const auto &v: mesh.get_vertices(f)) {
            center += positions[v].template cast<double>();
            ++count;
        }
        return (center / count).template cast<T>();
    }

    [[nodiscard]] Vector<Real, 3> FaceAreaVector(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f);

    [[nodiscard]] Vector<Real, 3> FaceNormal(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f);

    [[nodiscard]] Vector<Real, 3> FaceGradient(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f, VertexProperty<Real> scalarfield);

    [[nodiscard]] Vector<Real, 3> FaceToBarycentricCoordinates(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f, const Vector<Real, 3> &p);

    [[nodiscard]] Vector<Real, 3> FaceFromBarycentricCoordinates(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Face &f, const Vector<Real, 3> &bc);

    //------------------------------------------------------------------------------------------------------------------
    // Edge Methods
    //------------------------------------------------------------------------------------------------------------------

    template<typename T, int N>
    [[nodiscard]] Real EdgeLength(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Edge &e) {
        return static_cast<Real>((positions[mesh.get_vertex(e, 1)].template cast<double>() - positions[mesh.
                get_vertex(e, 0)].template cast<double>()).norm());
    }

    template<typename T, int N>
    [[nodiscard]] Vector<Real, 3>
    EdgeVector(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Edge &e) {
        return (positions[mesh.get_vertex(e, 1)].template cast<double>() -
                positions[mesh.get_vertex(e, 0)].template cast<double>()).template cast<Real>();
    }

    template<typename T, int N>
    [[nodiscard]] Vector<Real, 3>
    EdgeMidpoint(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Edge &e) {
        return ((positions[mesh.get_vertex(e, 0)].template cast<double>() +
                 positions[mesh.get_vertex(e, 1)].template cast<double>()) / 2.0).template cast<Real>();
    }

    [[nodiscard]] Real EdgeCotan(const Mesh &mesh, const Edge &e);

    //------------------------------------------------------------------------------------------------------------------
    // Vertex Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] Vector<Real, 3> VertexNormal(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Vertex &v);

    [[nodiscard]] Real VertexVoronoiMixedArea(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Vertex &v);

    [[nodiscard]] Real VertexBarycentricArea(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Vertex &v);

    template<typename T, int N>
    [[nodiscard]] Vector<T, N> VertexStarCenter(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Vertex &v) {
        Vector<double, N> center = Vector<double, N>::Zero();

        if (!mesh.is_isolated(v)) {
            int count = 0;
            for (const auto &vv: mesh.get_vertices(v)) {
                center += positions[vv].template cast<double>();
                ++count;
            }
            center /= count;
        }
        return center.template cast<Real>();
    }

    [[nodiscard]] Vector<Real, 3> VertexStarGradient(const Mesh &mesh, const VertexProperty<Vector<Real, 3>> &positions, const Vertex &v, VertexProperty<Real> scalarfield);

    template<typename T, int N>
    [[nodiscard]] Vector<T, N>
    VertexLaplace(const Mesh &mesh, const VertexProperty<Vector<T, N>> &positions, const Vertex &v, Real vertex_area) {
        Vector<double, N> laplace = Vector<double, N>::Zero();

        if (!mesh.is_isolated(v)) {
            double sum_weights(0.0);

            for (auto h: mesh.get_halfedges(v)) {
                const auto weight = EdgeCotan(mesh, mesh.get_edge(h));
                sum_weights += weight;
                laplace += weight * positions[mesh.get_vertex(h)].template cast<double>();
            }

            laplace -= sum_weights * positions[v].template cast<double>();
            laplace /= 2.0 * (vertex_area > 0 ? vertex_area : VertexBarycentricArea(mesh, positions, v));
        }

        return laplace.template cast<T>();
    }


}

#endif //MESHUTILS_H
