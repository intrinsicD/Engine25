//
// Created by alex on 1/17/25.
//
#include <gtest/gtest.h>
#include "Math.h"
#include "VoxelGridDownsampling.h"
#include "PointCloudUtils.h"

namespace Bcg {
    TEST(FlattenVoxelIndexTests, CompareLoopAndVectorized) {
        using Vector3i = Eigen::Vector3i;

        std::vector<std::pair<Vector3i, Vector3i>> test_cases = {
            {Vector3i(2, 3, 1), Vector3i(4, 5, 6)},
            {Vector3i(0, 0, 0), Vector3i(10, 10, 10)},
            {Vector3i(1, 1, 1), Vector3i(2, 3, 4)},
            {Vector3i(5, 2, 8), Vector3i(10, 10, 10)}
        };

        for (const auto& [idx, grid_dims] : test_cases) {
            size_t loop_result = FlattenVoxelIndexLoop<int, 3>(idx, grid_dims);
            size_t vectorized_result = FlattenVoxelIndexVectorized<int, 3>(idx, grid_dims);

            EXPECT_EQ(loop_result, vectorized_result)
                << "Mismatch between loop and vectorized implementations.\n"
                << "Index: " << idx.transpose() << "\n"
                << "Grid Dimensions: " << grid_dims.transpose() << "\n"
                << "Loop Result: " << loop_result << "\n"
                << "Vectorized Result: " << vectorized_result;
        }
    }

    TEST(VoxelGridContextTests, ComputeAABB) {
        std::vector<Eigen::Vector3f> points = {
            {0.1f, 0.2f, 0.3f},
            {1.0f, 1.5f, 1.2f},
            {0.4f, 0.5f, 0.6f}
        };

        Eigen::Vector3f voxel_size(0.5f, 0.5f, 0.5f);
        VoxelGridContext<float, 3> context(points, voxel_size);

        EXPECT_EQ(context.aabb.min(), Eigen::Vector3f(0.1f, 0.2f, 0.3f));
        EXPECT_EQ(context.aabb.max(), Eigen::Vector3f(1.0f, 1.5f, 1.2f));
    }

    TEST(VoxelGridContextTests, ComputeGridDimsAndStrides) {
        std::vector<Eigen::Vector3f> points = {
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f}
        };

        Eigen::Vector3f voxel_size(0.5f, 0.5f, 0.5f);
        VoxelGridContext<float, 3> context(points, voxel_size);

        EXPECT_EQ(context.grid_dims, Eigen::Vector3i(2, 2, 2));
        EXPECT_EQ(context.strides, Eigen::Vector3f(1.0f, 2.0f, 4.0f));
    }

    TEST(VoxelIndexTests, ComputeVoxelIndex) {
        Eigen::Vector3f point(0.75f, 1.25f, 0.5f);
        Eigen::Vector3f voxel_size(0.5f, 0.5f, 0.5f);

        Eigen::Vector3i expected_idx(1, 2, 1);
        Eigen::Vector3i computed_idx = VoxelIndex<float, 3>(point, voxel_size);

        EXPECT_EQ(computed_idx, expected_idx);
    }

    TEST(FillVoxelMapTests, PopulateVoxelMap) {
        std::vector<Eigen::Vector3f> points = {
            {0.1f, 0.2f, 0.3f},
            {0.4f, 0.5f, 0.6f},
            {1.0f, 1.5f, 1.2f}
        };

        Eigen::Vector3f voxel_size(0.5f, 0.5f, 0.5f);
        VoxelGridContext<float, 3> context(points, voxel_size);
        auto voxelMap = FillVoxelMap(points, context);

        EXPECT_EQ(voxelMap.size(), 3);
    }

    TEST(ExtractDownsampledPointsTests, ComputeCentroids) {
        std::unordered_map<size_t, std::pair<Eigen::Vector3f, int>> voxelMap = {
            {0, {Eigen::Vector3f(0.2f, 0.4f, 0.6f), 2}},
            {1, {Eigen::Vector3f(1.0f, 1.5f, 1.2f), 1}}
        };

        auto downsampled_points = ExtractDownsampledPoints(voxelMap);

        SortLexicographicalCoordinates(downsampled_points);

        EXPECT_EQ(downsampled_points.size(), 2);
        EXPECT_EQ(downsampled_points[0], Eigen::Vector3f(0.1f, 0.2f, 0.3f)); // Centroid of voxel 0
        EXPECT_EQ(downsampled_points[1], Eigen::Vector3f(1.0f, 1.5f, 1.2f)); // Centroid of voxel 1
    }

    TEST(VoxelGridDownsamplingTests, FullPipeline) {
        std::vector<Eigen::Vector3f> points = {
            {0.1f, 0.2f, 0.3f},
            {0.4f, 0.5f, 0.6f},
            {1.0f, 1.5f, 1.2f}
        };

        Eigen::Vector3f voxel_size(0.5f, 0.5f, 0.5f);
        auto downsampled_points = VoxelGridDownsampling(points, voxel_size);

        SortLexicographicalCoordinates(downsampled_points);

        EXPECT_EQ(downsampled_points.size(), 3);
        EXPECT_EQ(downsampled_points[0], Eigen::Vector3f(0.1f, 0.2f, 0.3f));
        EXPECT_EQ(downsampled_points[1], Eigen::Vector3f(0.4f, 0.5f, 0.6f));
        EXPECT_EQ(downsampled_points[2], Eigen::Vector3f(1.0f, 1.5f, 1.2f));
    }
}