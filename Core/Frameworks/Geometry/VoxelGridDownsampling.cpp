//
// Created by alex on 2/2/25.
//

#include "VoxelGridDownsampling.h"

namespace Bcg {
    VoxelGridDownsampling::VoxelGridDownsampling(const std::vector<Vector<Real, 3>> &positions) : positions(positions){
        integrated_positions = grid.voxel_property<Vector<Real, 3>>("v:integrated_position", Vector<Real, 3>::Zero());
        counts = grid.voxel_property("v:count", 0);
        assert(integrated_positions);
        assert(counts);
    }

    bool VoxelGridDownsampling::build_grid(const AABB<Real, 3> &aabb, const Vector<Real, 3> &voxel_sizes) {
        if (voxel_sizes.minCoeff() <= 0) {
            return false;
        }

        Vector<int, 3> grid_dims = GridDims(aabb, voxel_sizes);
        Vector<int, 3> strides = Strides(grid_dims);

        grid.voxels.reserve(grid_dims.prod());
        for (const auto &point: positions) {
            Vector<int, 3> voxelIdx = VoxelIndex(point, voxel_sizes);
            size_t linearIdx = VoxelLinearIndex(voxelIdx, strides);
            auto v = grid.add_voxel(linearIdx);
            integrated_positions[v] += point;
            counts[v] += 1;
        }

        return true;
    }

    std::vector<Vector<Real, 3>> VoxelGridDownsampling::downsampled_positions() {
        if (grid.is_empty()) {
            return {};
        }

        std::vector<Vector<Real, 3>> downsampled_positions;
        for (const auto &v: grid.voxels) {
            if (counts[v] > 0) {
                downsampled_positions.emplace_back(integrated_positions[v] / counts[v]);
            }
        }
        return downsampled_positions;
    }
}