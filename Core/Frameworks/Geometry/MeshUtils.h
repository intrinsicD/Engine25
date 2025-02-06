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

    [[nodiscard]] Mesh Dual(const Mesh &mesh);

    [[nodiscard]] Real VolumeTetrahedralDecomposition(const Mesh &mesh);

    [[nodiscard]] Real VolumeDivergenceTheorem(const Mesh &mesh);

    [[nodiscard]] Real SurfaceArea(const Mesh &mesh);

    //------------------------------------------------------------------------------------------------------------------
    // Face Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] Real FaceArea(const Mesh &mesh, const Face &f);

    [[nodiscard]] Vector<Real, 3> FaceCenter(const Mesh &mesh, const Face &f);

    [[nodiscard]] Vector<Real, 3> FaceAreaVector(const Mesh &mesh, const Face &f);

    [[nodiscard]] Vector<Real, 3> FaceNormal(const Mesh &mesh, const Face &f);

    [[nodiscard]] Vector<Real, 3> FaceGradient(const Mesh &mesh, const Face &f, VertexProperty<Real> scalarfield);

    //------------------------------------------------------------------------------------------------------------------
    // Vertex Methods
    //------------------------------------------------------------------------------------------------------------------

    [[nodiscard]] Vector<Real, 3> VertexNormal(const Mesh &mesh, const Vertex &v);

    [[nodiscard]] Real VertexVoronoiArea(const Mesh &mesh, const Vertex &v);

    [[nodiscard]] Real VertexBarycentricArea(const Mesh &mesh, const Vertex &v);

    [[nodiscard]] Vector<Real, 3> VertexStarCenter(const Mesh &mesh, const Vertex &v);

    [[nodiscard]] Vector<Real, 3> VertexStarGradient(const Mesh &mesh, const Vertex &v, VertexProperty<Real> scalarfield);


}

#endif //MESHUTILS_H
