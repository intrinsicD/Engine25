//
// Created by alex on 1/17/25.
//

#ifndef POINTCLOUDUTILS_H
#define POINTCLOUDUTILS_H

#include "GeometricProperties.h"

namespace Bcg {
    /**
     * @brief Sorts a vector of N-dimensional points lexicographically.
     *
     * This function sorts the given vector of points in lexicographical order.
     * Lexicographical order means that the points are compared element by element,
     * starting from the first element. If the first elements are equal, the second
     * elements are compared, and so on.
     *
     * @tparam T The type of the coordinates (e.g., float, double).
     * @tparam N The number of dimensions.
     * @param points The vector of points to be sorted.
     */
    template<typename T, int N>
    void SortLexicographicalCoordinates(std::vector<Vector<T, N>> &points) {
        // Sort the points for consistent ordering
        std::sort(points.begin(), points.end(), [](const Vector<T, N> &a, const Vector<T, N> &b) {
            for (int i = 0; i < N; ++i) {
                if (a[i] != b[i]) return a[i] < b[i];
            }
            return false;
        });
    }

    /**
     * @brief Computes the center of a set of N-dimensional points.
     *
     * This function calculates the centroid of the given vector of points.
     * The centroid is the arithmetic mean of all the points.
     *
     * @tparam T The type of the coordinates (e.g., float, double).
     * @tparam N The number of dimensions.
     * @param positions The vector of points for which to compute the center.
     * @return The center point of the given points.
     */
    template<typename T, int N>
    Vector<T, N> Center(const std::vector<Vector<T, N>> &positions) {
        Vector<double, N> center = Vector<double, N>::Zero();
        for (const Vector<T, N> &v: positions) {
            center += v.template cast<double>();
        }
        return (center / static_cast<double>(positions.size())).template cast<T>();
    }
}

#endif //POINTCLOUDUTILS_H
