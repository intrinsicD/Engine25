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

    [[nodiscard]] Real VolumeTetrahedralDecomposition(const Mesh &mesh);

    [[nodiscard]] Real VolumeDivergenceTheorem(const Mesh &mesh);

    [[nodiscard]] Real SurfaceArea(const Mesh &mesh);

    [[nodiscard]] Vector<Real, 3> Centroid(const Mesh &mesh);

    //------------------------------------------------------------------------------------------------------------------
    // Face Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] Real FaceArea(const Mesh &mesh, const Face &f);

    [[nodiscard]] Vector<Real, 3> FaceCenter(const Mesh &mesh, const Face &f);

    [[nodiscard]] Vector<Real, 3> FaceAreaVector(const Mesh &mesh, const Face &f);

    [[nodiscard]] Vector<Real, 3> FaceNormal(const Mesh &mesh, const Face &f);

    [[nodiscard]] Vector<Real, 3> FaceGradient(const Mesh &mesh, const Face &f, VertexProperty<Real> scalarfield);

    //------------------------------------------------------------------------------------------------------------------
    // Edge Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] Real EdgeLength(const Mesh &mesh, const Edge &e);

    [[nodiscard]] Vector<Real, 3> EdgeVector(const Mesh &mesh, const Edge &e);

    [[nodiscard]] Vector<Real, 3> EdgeMidpoint(const Mesh &mesh, const Edge &e);

    [[nodiscard]] Real EdgeCotan(const Mesh &mesh, const Edge &e);

    //------------------------------------------------------------------------------------------------------------------
    // Vertex Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] Vector<Real, 3> VertexNormal(const Mesh &mesh, const Vertex &v);

    [[nodiscard]] Real VertexVoronoiMixedArea(const Mesh &mesh, const Vertex &v);

    [[nodiscard]] Real VertexBarycentricArea(const Mesh &mesh, const Vertex &v);

    [[nodiscard]] Vector<Real, 3> VertexStarCenter(const Mesh &mesh, const Vertex &v);

    [[nodiscard]] Vector<Real, 3> VertexStarGradient(const Mesh &mesh, const Vertex &v, VertexProperty<Real> scalarfield);

    [[nodiscard]] Vector<Real, 3> VertexLaplace(const Mesh &mesh, const Vertex &v, Real vertex_area = 0);


}

#endif //MESHUTILS_H
