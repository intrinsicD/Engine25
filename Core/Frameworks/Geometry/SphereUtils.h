//
// Created by alex on 1/7/25.
//

#ifndef SPHEREUTILS_H
#define SPHEREUTILS_H

#include "Sphere.h"

namespace Bcg{
/**
 * @brief Merges two spheres into a single sphere that bounds both.
 * @tparam T The type of the coordinates (e.g., float, double).
 * @tparam N The number of dimensions.
 * @param lhs The first sphere.
 * @param rhs The second sphere.
 * @return A new sphere that bounds both input spheres.
 */
template<typename T, int N>
CUDA_HOST_DEVICE
Sphere<T, N> Merge(const Sphere<T, N> &lhs, const Sphere<T, N> &rhs) noexcept {
    Vector<T, N> direction = rhs.center() - lhs.center();
    T distance = direction.norm();
    if (distance <= std::abs(lhs.radius() - rhs.radius())) {
        // One sphere fully contains the other
        return lhs.radius() > rhs.radius() ? lhs : rhs;
    }

    T new_radius = (distance + lhs.radius() + rhs.radius()) / 2.0;
    Vector<T, N> new_center = lhs.center() + direction / distance * (new_radius - lhs.radius());
    return {new_center, new_radius};
}

/**
 * @brief Checks if two spheres intersect.
 * @tparam T The type of the coordinates (e.g., float, double).
 * @tparam N The number of dimensions.
 * @param lhs The first sphere.
 * @param rhs The second sphere.
 * @return True if the spheres intersect, false otherwise.
 */
template<typename T, int N>
CUDA_HOST_DEVICE
bool Intersects(const Sphere<T, N> &lhs, const Sphere<T, N> &rhs) noexcept {
    return (lhs.center() - rhs.center()).norm() <= lhs.radius() + rhs.radius();
}

/**
 * @brief Checks if one sphere completely contains another sphere.
 * @tparam T The type of the coordinates (e.g., float, double).
 * @tparam N The number of dimensions.
 * @param lhs The first sphere.
 * @param rhs The second sphere.
 * @return True if the first sphere contains the second sphere, false otherwise.
 */
template<typename T, int N>
CUDA_HOST_DEVICE
bool Contains(const Sphere<T, N> &lhs, const Sphere<T, N> &rhs) noexcept {
    return (lhs.center() - rhs.center()).norm() + rhs.radius() <= lhs.radius();
}

/**
 * @brief Finds the closest point on the surface of the sphere to a given point.
 * @tparam T The type of the coordinates (e.g., float, double).
 * @tparam N The number of dimensions.
 * @param sphere The sphere.
 * @param point The point to find the closest point to.
 * @return The closest point on the surface of the sphere.
 */
template<typename T, int N>
CUDA_HOST_DEVICE
Vector<T, N> ClosestPoint(const Sphere<T, N> &sphere, const Vector<T, N> &point) {
    Vector<T, N> direction = point - sphere.center();
    T distance = direction.norm();
    if (distance == 0) {
        return sphere.center();
    }
    return sphere.center() + direction / distance * sphere.radius();
}

/**
 * @brief Computes the minimum distance from a point to the surface of the sphere.
 * @tparam T The type of the coordinates (e.g., float, double).
 * @tparam N The number of dimensions.
 * @param sphere The sphere.
 * @param point The point to compute the distance from.
 * @return The minimum distance from the point to the surface of the sphere.
 */
template<typename T, int N>
CUDA_HOST_DEVICE
T MinSqDist(const Sphere<T, N> &sphere, const Vector<T, N> &point) {
    return (ClosestPoint(sphere, point) - point).squaredNorm();
}

/**
 * @brief Computes the maximum of the minimum distance and the radius of the sphere.
 * @tparam T The type of the coordinates (e.g., float, double).
 * @tparam N The number of dimensions.
 * @param sphere The sphere.
 * @param point The point to compute the distance from.
 * @return The maximum of the minimum distance and the radius of the sphere.
 */
template<typename T, int N>
CUDA_HOST_DEVICE
T MinMaxSqDist(const Sphere<T, N> &sphere, const Vector<T, N> &point) {
    return std::max((ClosestPoint(sphere, point) - point).squaredNorm(), sphere.radius() * sphere.radius());
}
}

#endif //SPHEREUTILS_H
