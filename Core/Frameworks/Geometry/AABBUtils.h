//
// Created by alex on 1/7/25.
//

#ifndef AABBUTILS_H
#define AABBUTILS_H

#include "AABB.h"

namespace Bcg{
    // Merges two AABBs into a single AABB that encompasses both
    template<typename T, int N>
    CUDA_HOST_DEVICE
    AABB<T, N> Merge(const AABB<T, N> &lhs, const AABB<T, N> &rhs) noexcept {
        return {lhs.min().cwiseMin(rhs.min()), lhs.max().cwiseMax(rhs.max())};
    }

    // Checks if two AABBs intersect
    template<typename T, int N>
    CUDA_HOST_DEVICE
    bool Intersects(const AABB<T, N> &lhs, const AABB<T, N> &rhs) noexcept {
        for (int i = 0; i < lhs.size(); ++i) {
            if (lhs.max()(i) < rhs.min()(i) || rhs.max()(i) < lhs.min()(i)) {
                return false;
            }
        }
        return true;
    }

    // Computes the closest point on the AABB to a given point
    template<typename T, int N>
    CUDA_HOST_DEVICE
    Vector<T, N> ClosestPoint(const AABB<T, N> &aabb, const Vector<T, N> &point) {
        return point.cwiseMax(aabb.min()).cwiseMin(aabb.max());
    }

    // Computes the minimum squared distance from a point to the AABB
    // Defined as MINDIST in "Nearest Neighbor Queries (1995)"
    template<typename T, int N>
    CUDA_HOST_DEVICE
    T MinSqDist(const AABB<T, N> &aabb, const Vector<T, N> &point) {
        return (ClosestPoint(aabb, point) - point).squaredNorm();
    }

    // Computes the minimum maximum squared distance from a point to the AABB
    // Defined as MINMAXDIST in "Nearest Neighbor Queries (1995)"
    template<typename T, int N>
    CUDA_HOST_DEVICE
    T MinMaxSqDist(const AABB<T, N> &aabb, const Vector<T, N> &point) {
        // Squared differences between the point and AABB bounds
        Vector<T, N> lower_diff_sq = (aabb.min() - point).array().square();
        Vector<T, N> upper_diff_sq = (aabb.max() - point).array().square();

        // Compute the center of the AABB
        Vector<T, N> center = aabb.center();

        // Initialize minimum maximum distance
        T min_max_dist = std::numeric_limits<T>::infinity();

        // Iterate over each dimension to compute MINMAXDIST
        for (int i = 0; i < N; ++i) {
            // Compute the "R" terms (distance contributions) for this dimension
            T rm_sq = lower_diff_sq[i];
            T rM_sq = upper_diff_sq[i];

            // Swap if the point is to the right of the center
            if (point[i] > center[i]) {
                std::swap(rm_sq, rM_sq);
            }

            // Calculate the sum of squared distances for this configuration
            T sum_sq = rm_sq;
            for (int j = 0; j < N; ++j) {
                if (j != i) {
                    sum_sq += (point[j] < center[j]) ? upper_diff_sq[j] : lower_diff_sq[j];
                }
            }

            // Update the minimum maximum distance
            min_max_dist = std::min(min_max_dist, sum_sq);
        }

        return min_max_dist; // Return squared distance for efficiency
    }
}

#endif //AABBUTILS_H
