//
// Created by alex on 10.01.25.
//

// TestPointCloud.cpp
#include "gtest/gtest.h"
#include "PointCloud.h"

using namespace Bcg;

// Test fixture for PointCloud
class PointCloudTest : public ::testing::Test {
protected:
    PointCloud pc;
};

TEST_F(PointCloudTest, DefaultConstructor) {
    EXPECT_TRUE(pc.is_empty());
    EXPECT_EQ(pc.n_vertices(), 0);
    EXPECT_FALSE(pc.has_garbage());
}

TEST_F(PointCloudTest, AddVertex) {
    auto vertex = pc.add_vertex(Vector<Real, 3>(1.0, 2.0, 3.0));
    EXPECT_TRUE(vertex.is_valid());
    EXPECT_EQ(pc.n_vertices(), 1);
    EXPECT_EQ(pc.positions[vertex], (Vector<Real, 3>(1.0, 2.0, 3.0)));
}

TEST_F(PointCloudTest, MarkAndDeleteVertex) {
    auto vertex = pc.add_vertex(Vector<Real, 3>(1.0, 2.0, 3.0));
    pc.mark_deleted(vertex);
    EXPECT_TRUE(pc.is_deleted(vertex));
    pc.delete_vertex(vertex);
    EXPECT_TRUE(pc.is_deleted(vertex));
    EXPECT_EQ(pc.vertices.size(), 1); // Deleted vertices still count until garbage collection.
    EXPECT_EQ(pc.n_vertices(), 0);
}

TEST_F(PointCloudTest, GarbageCollection) {
    auto v1 = pc.add_vertex(Vector<Real, 3>(1.0, 2.0, 3.0));
    auto v2 = pc.add_vertex(Vector<Real, 3>(4.0, 5.0, 6.0));
    pc.mark_deleted(v1);

    pc.garbage_collection();

    // Ensure only valid positions remain after garbage collection.
    EXPECT_EQ(pc.n_vertices(), 1);

    // Check if the remaining vertex data is correct.
    auto validVertices = pc.positions.vector();
    ASSERT_EQ(validVertices.size(), 1);
    EXPECT_EQ(validVertices[0], (Vector<Real, 3>(4.0, 5.0, 6.0)));

    EXPECT_FALSE(pc.has_garbage());
}

TEST_F(PointCloudTest, ClearAndReserve) {
    pc.add_vertex(Vector<Real, 3>(1.0, 2.0, 3.0));
    pc.add_vertex(Vector<Real, 3>(4.0, 5.0, 6.0));
    pc.clear();

    EXPECT_TRUE(pc.is_empty());
    EXPECT_EQ(pc.n_vertices(), 0);

    pc.reserve(100);
    EXPECT_EQ(pc.n_vertices(), 0); // Reserving does not add vertices.
}
