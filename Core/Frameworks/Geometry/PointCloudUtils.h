//
// Created by alex on 1/17/25.
//

#ifndef POINTCLOUDUTILS_H
#define POINTCLOUDUTILS_H

#include "Math.h"

namespace Bcg{
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
