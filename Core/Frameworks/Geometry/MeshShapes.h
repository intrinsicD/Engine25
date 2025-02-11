//
// Created by alex on 2/10/25.
//

#ifndef MESHSHAPES_H
#define MESHSHAPES_H

#include "Mesh.h"

namespace Bcg {
    /**
     * @brief Generates a mesh representing a vertex one-ring.
     * @return A mesh representing a vertex one-ring.
     */
    Mesh VertexOneRing();

    /**
     * @brief Generates a mesh representing an edge one-ring.
     * @return A mesh representing an edge one-ring.
     */
    Mesh EdgeOneRing();

    /**
     * @brief Generates a mesh representing an L-shaped structure.
     * @return A mesh representing an L-shaped structure.
     */
    Mesh LShape();

    /**
     * @brief Generates a mesh representing an open cone.
     * @return A mesh representing an open cone.
     */
    Mesh OpenCone();

    /**
     * @brief Generates a mesh with texture seams.
     * @return A mesh with texture seams.
     */
    Mesh TextureSeamsMesh();

    /**
     * @brief Generates a mesh representing a tetrahedron.
     * @return A mesh representing a tetrahedron.
     */
    Mesh Tetrahedron();

    /**
     * @brief Generates a mesh representing a hexahedron.
     * @return A mesh representing a hexahedron.
     */
    Mesh Hexahedron();

    /**
     * @brief Generates a mesh representing an octahedron.
     * @return A mesh representing an octahedron.
     */
    Mesh Octahedron();

    /**
     * @brief Generates a mesh representing a dodecahedron.
     * @return A mesh representing a dodecahedron.
     */
    Mesh Dodecahedron();

    /**
     * @brief Generates a mesh representing an icosahedron.
     * @return A mesh representing an icosahedron.
     */
    Mesh Icosahedron();

    /**
     * @brief Generates a mesh representing an icosphere with a specified number of subdivisions.
     * @param n_subdivisions The number of subdivisions for the icosphere.
     * @return A mesh representing an icosphere.
     */
    Mesh Icosphere(size_t n_subdivisions = 3);

    /**
     * @brief Generates a mesh representing a quad sphere with a specified number of subdivisions.
     * @param n_subdivisions The number of subdivisions for the quad sphere.
     * @return A mesh representing a quad sphere.
     */
    Mesh QuadSphere(size_t n_subdivisions = 3);

    /**
     * @brief Generates a mesh representing a UV sphere with specified parameters.
     * @param center The center of the UV sphere.
     * @param radius The radius of the UV sphere.
     * @param n_slices The number of slices for the UV sphere.
     * @param n_stacks The number of stacks for the UV sphere.
     * @return A mesh representing a UV sphere.
     */
    Mesh UVSphere(const Vector<Real, 3> &center = Vector<Real, 3>::Zero(), Real radius = 1.0, size_t n_slices = 15,
                  size_t n_stacks = 15);

    /**
     * @brief Generates a mesh representing a plane with a specified resolution.
     * @param resolution The resolution of the plane.
     * @return A mesh representing a plane.
     */
    Mesh Plane(size_t resolution = 4);

    /**
     * @brief Generates a mesh representing a cone with specified parameters.
     * @param n_subdivisions The number of subdivisions for the cone.
     * @param radius The radius of the cone.
     * @param height The height of the cone.
     * @return A mesh representing a cone.
     */
    Mesh Cone(size_t n_subdivisions = 30, Real radius = 1.0, Real height = 2.5);

    /**
     * @brief Generates a mesh representing a cylinder with specified parameters.
     * @param n_subdivisions The number of subdivisions for the cylinder.
     * @param radius The radius of the cylinder.
     * @param height The height of the cylinder.
     * @return A mesh representing a cylinder.
     */
    Mesh Cylinder(size_t n_subdivisions = 30, Real radius = 1.0, Real height = 2.5);

    /**
     * @brief Generates a mesh representing a torus with specified parameters.
     * @param radial_resolution The radial resolution of the torus.
     * @param tubular_resolution The tubular resolution of the torus.
     * @param radius The radius of the torus.
     * @param thickness The thickness of the torus.
     * @return A mesh representing a torus.
     */
    Mesh Torus(size_t radial_resolution = 20, size_t tubular_resolution = 40,
               Real radius = 1.0, Real thickness = 0.4);
};

#endif //MESHSHAPES_H
