//
// Created by alex on 1/18/25.
//
#include <gtest/gtest.h>
#include "VoxelGrid.h"

using namespace Bcg;

class VoxelGridTest : public ::testing::Test {
protected:
    VoxelGrid grid;

    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(VoxelGridTest, VoxelSizes_ComputesCorrectSizes) {
    AABB<float, 3> aabb(Vector<Real, 3>(0, 0, 0), Vector<Real, 3>(10, 10, 10));
    Vector<int, 3> grid_dims(5, 5, 5);
    Vector<Real, 3> expected_sizes(2, 2, 2);
    EXPECT_EQ(VoxelSizes(aabb, grid_dims), expected_sizes);
}

TEST_F(VoxelGridTest, GridDims_ComputesCorrectDimensions) {
    AABB<float, 3> aabb(Vector<Real, 3>(0, 0, 0), Vector<Real, 3>(10, 10, 10));
    Vector<Real, 3> voxel_size(2, 2, 2);
    Vector<int, 3> expected_dims(5, 5, 5);
    EXPECT_EQ(GridDims(aabb, voxel_size), expected_dims);
}

TEST_F(VoxelGridTest, Strides_ComputesCorrectStrides) {
    Vector<int, 3> grid_dims(5, 5, 5);
    Vector<int, 3> expected_strides(1, 5, 25);
    EXPECT_EQ(Strides(grid_dims), expected_strides);
}

TEST_F(VoxelGridTest, ComputesCorrectIndexForPoint) {
    Vector<float, 3> point(1.5, 2.5, 3.5);
    Vector<float, 3> voxel_size(1.0, 1.0, 1.0);
    Vector<int, 3> expected_index(1, 2, 3);
    Vector<int, 3> index = VoxelIndex(point, voxel_size);
    EXPECT_EQ(index, expected_index);
}

TEST_F(VoxelGridTest, HandlesZeroVoxelSize) {
    Vector<float, 3> point(1.5, 2.5, 3.5);
    Vector<float, 3> voxel_size(0.0, 0.0, 0.0);
    Vector<int, 3> expected_index(0, 0, 0);
    Vector<int, 3> index = VoxelIndex(point, voxel_size);
    EXPECT_EQ(index, expected_index);
}

TEST_F(VoxelGridTest, HandlesNegativeCoordinates) {
    Vector<float, 3> point(-1.5, -2.5, -3.5);
    Vector<float, 3> voxel_size(1.0, 1.0, 1.0);
    Vector<int, 3> expected_index(-1, -2, -3);
    Vector<int, 3> index = VoxelIndex(point, voxel_size);
    EXPECT_EQ(index, expected_index);
}

TEST_F(VoxelGridTest, HandlesNonUniformVoxelSize) {
    Vector<float, 3> point(1.5, 2.5, 3.5);
    Vector<float, 3> voxel_size(0.5, 1.0, 2.0);
    Vector<int, 3> expected_index(3, 2, 1);
    Vector<int, 3> index = VoxelIndex(point, voxel_size);
    EXPECT_EQ(index, expected_index);
}

TEST_F(VoxelGridTest, VoxelLinearIndex_ComputesCorrectIndex) {
    Vector<int, 3> idx(1, 2, 3);
    Vector<int, 3> strides(1, 5, 25);
    size_t expected_index = 86;
    EXPECT_EQ(VoxelLinearIndex(idx, strides), expected_index);
}

TEST_F(VoxelGridTest, VoxelIndex_ComputesCorrectMultiDimensionalIndex) {
    size_t linear_index = 86;
    Vector<int, 3> strides(1, 5, 25);
    Vector<int, 3> expected_idx(1, 2, 3);
    EXPECT_EQ(VoxelIndex(linear_index, strides), expected_idx);
}

TEST_F(VoxelGridTest, NewVoxel_CreatesVoxel) {
    Voxel v = grid.new_voxel();
    EXPECT_EQ(v.idx(), 0);
    EXPECT_EQ(grid.n_voxels(), 1);
}

TEST_F(VoxelGridTest, AddVoxel_AddsVoxelAtIndex) {
    Vector<int, 3> idx(1, 2, 3);
    Vector<int, 3> strides(1, 5, 25);
    Voxel v = grid.add_voxel<3>(idx, strides);
    EXPECT_EQ(v.idx(), 0);
    EXPECT_EQ(grid.n_voxels(), 1);
}

TEST_F(VoxelGridTest, MarkDeleted_MarksVoxelAsDeleted) {
    Voxel v = grid.new_voxel();
    grid.mark_deleted(v);
    EXPECT_TRUE(grid.is_deleted(v));
}

TEST_F(VoxelGridTest, DeleteVoxel_DeletesVoxel) {
    Vector<int, 3> idx(1, 2, 3);
    Vector<int, 3> strides(1, 5, 25);
    Voxel v = grid.add_voxel<3>(idx, strides);
    grid.delete_voxel(v, VoxelLinearIndex(idx, strides));
    EXPECT_EQ(grid.n_voxels(), 0);
    EXPECT_TRUE(grid.is_deleted(v));
    grid.garbage_collection();
    EXPECT_EQ(grid.n_voxels(), 0);
}