//
// Created by alex on 1/17/25.
//
#include <gtest/gtest.h>
#include "Math.h"
#include "VoxelGridDownsampling.h"

namespace Bcg {
    TEST(VoxelGridDownsampling, BuildsGridSuccessfully) {
        std::vector<Vector<Real, 3>> positions = {Vector<Real, 3>(1.0, 2.0, 3.0), Vector<Real, 3>(4.0, 5.0, 6.0)};
        VoxelGridDownsampling downsampling(positions);
        AABB<Real, 3> aabb(Vector<Real, 3>(0.0, 0.0, 0.0), Vector<Real, 3>(10.0, 10.0, 10.0));
        Vector<Real, 3> voxel_sizes(1.0, 1.0, 1.0);
        EXPECT_TRUE(downsampling.build_grid(aabb, voxel_sizes));
    }

    TEST(VoxelGridDownsampling, FailsToBuildGridWithInvalidVoxelSizes) {
        std::vector<Vector<Real, 3>> positions = {Vector<Real, 3>(1.0, 2.0, 3.0), Vector<Real, 3>(4.0, 5.0, 6.0)};
        VoxelGridDownsampling downsampling(positions);
        AABB<Real, 3> aabb(Vector<Real, 3>(0.0, 0.0, 0.0), Vector<Real, 3>(10.0, 10.0, 10.0));
        Vector<Real, 3> voxel_sizes(0.0, 1.0, 1.0);
        EXPECT_FALSE(downsampling.build_grid(aabb, voxel_sizes));
    }

    TEST(VoxelGridDownsampling, ReturnsEmptyDownsampledPositionsWhenGridIsEmpty) {
        std::vector<Vector<Real, 3>> positions = {};
        VoxelGridDownsampling downsampling(positions);
        std::vector<Vector<Real, 3>> downsampled_positions = downsampling.downsampled_positions();
        EXPECT_TRUE(downsampled_positions.empty());
    }

    TEST(VoxelGridDownsampling, ReturnsCorrectDownsampledPositions) {
        std::vector<Vector<Real, 3>> positions = {Vector<Real, 3>(1.0, 2.0, 3.0), Vector<Real, 3>(1.0, 2.0, 3.0)};
        VoxelGridDownsampling downsampling(positions);
        AABB<Real, 3> aabb(Vector<Real, 3>(0.0, 0.0, 0.0), Vector<Real, 3>(10.0, 10.0, 10.0));
        Vector<Real, 3> voxel_sizes(1.0, 1.0, 1.0);
        downsampling.build_grid(aabb, voxel_sizes);
        std::vector<Vector<Real, 3>> downsampled_positions = downsampling.downsampled_positions();
        EXPECT_EQ(downsampled_positions.size(), 1);
        EXPECT_EQ(downsampled_positions[0], (Vector<Real, 3>(1.0, 2.0, 3.0)));
    }
}