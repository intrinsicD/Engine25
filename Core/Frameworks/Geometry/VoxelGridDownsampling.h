//
// Created by alex on 1/17/25.
//

#ifndef VOXELGRIDDOWNSAMPLING_H
#define VOXELGRIDDOWNSAMPLING_H

#include <vector>
#include <unordered_map>
#include "AABB.h"

namespace Bcg{
    template<typename T, int N>
    struct VoxelGridContext {
        AABB<T, N> aabb;
        Vector<int, N> grid_dims;
        Vector<T, N> strides;
        Vector<T, N> voxel_size;

        VoxelGridContext(const std::vector<Vector<T, N>>& points, const Vector<T, N>& voxel_size)
            : voxel_size(voxel_size) {
            ComputeAABB(points);
            ComputeGridDims();
            ComputeStrides();
        }

        private:
          void ComputeAABB(const std::vector<Vector<T, N>> &points) {
            aabb = AABB(points.begin(), points.end());
          }

          void ComputeGridDims() {
            grid_dims = ((aabb.max - aabb.min).array() / voxel_size.array()).ceil().template cast<int>();
          }

          void ComputeStrides() {
            strides = Vector<T, N>::Ones();
              if (grid_dims.size() > 1) {
                  strides.tail(N - 1) = CumulativeProduct<int, N-1>(grid_dims.head(N - 1)).template cast<T>();
              }
          }
    };

    template<typename T, int N>
    size_t FlattenVoxelIndexLoop(const Vector<int, N>& idx, const Vector<int, N>& grid_dims) {
        size_t key = 0;
        size_t multiplier = 1;
        for (int i = 0; i < N; ++i) {
            key += idx[i] * multiplier;
            multiplier *= grid_dims[i];
        }
        return key;
    }

    template<typename T, int N>
    size_t FlattenVoxelIndexVectorized(const Vector<int, N>& idx, const Vector<int, N>& grid_dims) {
        // Compute cumulative strides for flattening
        Vector<T, N> strides = Vector<T, N>::Ones();
        if (grid_dims.size() > 1) {
            strides.tail(N - 1) = CumulativeProduct<int, N-1>(grid_dims.head(N - 1)).template cast<T>();
        }

        // Flattened index as dot product of idx and strides
        return idx.dot(strides);
    }

    template<typename T, int N>
    Vector<int, N> VoxelIndex(const Vector<T, N> &point, const Vector<T, N> &voxel_size) {
        return (point.array() / voxel_size.array() + T(1e-6)).template cast<int>();
    }

    template<typename T, int N>
    std::unordered_map<size_t, std::pair<Vector<T, N>, int>> FillVoxelMap(const std::vector<Vector<T, N>> &points,
                                                                          const VoxelGridContext<T, N> &context) {
        std::unordered_map<size_t, std::pair<Vector<T, N>, int>> voxelMap;
        voxelMap.reserve(context.grid_dims.prod());
        for (const auto& point : points) {
            // Compute voxel index. Points  might be outside the grid. Can be fixed by clamping to the aabb.
            Vector<int, N> voxelIdx = VoxelIndex(point, context.voxel_size);
            size_t key = voxelIdx.transpose().template cast<T>() * context.strides; // FlattenVoxelIndexVectorized but with strides precomputed
            auto [it, inserted] = voxelMap.try_emplace(key, point, 1); // Insert or get iterator
            if (!inserted) {
                it->second.first += point; // Accumulate position
                it->second.second += 1;    // Increment count
            }
        }
        return voxelMap;
    }

    template<typename T, int N>
    std::vector<Vector<T, N>> ExtractDownsampledPoints(const std::unordered_map<size_t, std::pair<Vector<T, N>, int>> &voxelMap) {
        std::vector<Vector<T, N>> downsampled_points;
        downsampled_points.reserve(voxelMap.size());
        for (const auto& [voxelIdx, data] : voxelMap) {
            downsampled_points.push_back(data.first / static_cast<T>(data.second)); // Compute average
        }
        return downsampled_points;
    }

    template<typename T, int N>
    std::vector<Vector<T, N>> VoxelGridDownsampling(const std::vector<Vector<T, N>> &points, const Vector<T, N> &voxel_size) {
        if(voxel_size.minCoeff() <= 0) {
            throw std::invalid_argument("voxel_size must be greater than 0 in all dimensions.");
        }
        if(points.empty()) {
            return {};
        }

        auto context = VoxelGridContext<T, N>(points, voxel_size);
        auto voxelMap = FillVoxelMap(points, context);
        return ExtractDownsampledPoints(voxelMap);
    }
}

#endif //VOXELGRIDDOWNSAMPLING_H
