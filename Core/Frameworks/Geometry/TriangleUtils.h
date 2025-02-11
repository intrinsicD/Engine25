//
// Created by alex on 2/10/25.
//

#ifndef TRIANGLEUTILS_H
#define TRIANGLEUTILS_H

#include "Math.h"

namespace Bcg {
/**
     * @brief Calculates the area of a triangle given its three vertices.
     * @param a The first vertex of the triangle.
     * @param b The second vertex of the triangle.
     * @param c The third vertex of the triangle.
     * @return The area of the triangle.
     */
    Real TriangleArea(const Vector<Real, 3> &a, const Vector<Real, 3> &b, const Vector<Real, 3> &c);

    /**
     * @brief Converts a point to barycentric coordinates with respect to a triangle.
     * @param p The point to convert.
     * @param u The first vertex of the triangle.
     * @param v The second vertex of the triangle.
     * @param w The third vertex of the triangle.
     * @return The barycentric coordinates of the point.
     */
    Vector<Real, 3> ToBarycentricCoordinates(const Vector<Real, 3> &p, const Vector<Real, 3> &u, const Vector<Real, 3> &v,
                                             const Vector<Real, 3> &w);

    /**
     * @brief Converts barycentric coordinates to Cartesian coordinates with respect to a triangle.
     * @param barycentric The barycentric coordinates to convert.
     * @param u The first vertex of the triangle.
     * @param v The second vertex of the triangle.
     * @param w The third vertex of the triangle.
     * @return The Cartesian coordinates of the point.
     */
    Vector<Real, 3> FromBarycentric(const Vector<Real, 3> &barycentric, const Vector<Real, 3> &u, const Vector<Real, 3> &v,
                                    const Vector<Real, 3> &w);
}

#endif //TRIANGLEUTILS_H
