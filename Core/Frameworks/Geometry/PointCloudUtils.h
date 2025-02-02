//
// Created by alex on 1/17/25.
//

#ifndef POINTCLOUDUTILS_H
#define POINTCLOUDUTILS_H

#include "Math.h"

namespace Bcg{
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
    std::sort(points.begin(), points.end(), [](const Vector<T, N>& a, const Vector<T, N>& b) {
        for (int i = 0; i < N; ++i) {
            if (a[i] != b[i]) return a[i] < b[i];
        }
        return false;
    });
}
}

#endif //POINTCLOUDUTILS_H
