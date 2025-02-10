//
// Created by alex on 2/10/25.
//

#ifndef MESHSUBDIVISION_H
#define MESHSUBDIVISION_H

#include "Mesh.h"

namespace Bcg::Subdivision {
    /**
     * @brief Enum class for handling boundary conditions in mesh subdivision.
     */
    enum class BoundaryHandling {
        Interpolate, /**< Interpolate boundary vertices. */
        Preserve /**< Preserve boundary vertices. */
    };

    /**
     * @brief Performs Catmull-Clark subdivision on the given mesh.
     * @param mesh The mesh to subdivide.
     * @param boundary_handling Determines how boundary vertices are treated.
     */
    void CatmullClark(Mesh &mesh, BoundaryHandling boundary_handling = BoundaryHandling::Interpolate);

    /**
     * @brief Performs Loop subdivision on the given mesh.
     * @param mesh The mesh to subdivide.
     * @param boundary_handling Determines how boundary vertices are treated.
     */
    void Loop(Mesh &mesh, BoundaryHandling boundary_handling = BoundaryHandling::Interpolate);

    /**
     * @brief Performs Quad-Tri subdivision on the given mesh.
     * @param mesh The mesh to subdivide.
     * @param boundary_handling Determines how boundary vertices are treated.
     */
    void QuadTri(Mesh &mesh, BoundaryHandling boundary_handling = BoundaryHandling::Interpolate);

    /**
     * @brief Performs Linear subdivision on the given mesh.
     * @param mesh The mesh to subdivide.
     */
    void Linear(Mesh &mesh);
};

#endif //MESHSUBDIVISION_H
