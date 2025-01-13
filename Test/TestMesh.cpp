//
// Created by alex on 13.01.25.
//

// TestGraph.cpp
#include "gtest/gtest.h"
#include "Mesh.h"

using namespace Bcg;

// Test fixture for Graph
class MeshTest : public ::testing::Test {
protected:
    Mesh mesh;
};

TEST_F(MeshTest, DefaultConstructor) {
    EXPECT_TRUE(mesh.is_empty());
    EXPECT_FALSE(mesh.has_garbage());
}

// Unit Test Examples
TEST_F(MeshTest, AddRemoveVertexTest) {
    auto v = mesh.add_vertex({1.0, 2.0, 3.0});
    ASSERT_TRUE(v.is_valid());
    EXPECT_EQ(mesh.n_vertices(), 1);
    EXPECT_EQ(mesh.positions[v], (Vector<Real, 3>(1.0, 2.0, 3.0)));
    mesh.delete_vertex(v);
    EXPECT_EQ(mesh.n_vertices(), 0);
    EXPECT_TRUE(mesh.has_garbage());
    mesh.garbage_collection();
    EXPECT_EQ(mesh.n_vertices(), 0);
    EXPECT_FALSE(mesh.has_garbage());
}

TEST_F(MeshTest, AddTriangleTest) {
    auto v0 = mesh.add_vertex({0.0, 0.0, 0.0});
    auto v1 = mesh.add_vertex({1.0, 0.0, 0.0});
    auto v2 = mesh.add_vertex({0.0, 1.0, 0.0});

    auto f = mesh.add_triangle(v0, v1, v2);
    ASSERT_TRUE(f.is_valid());

    EXPECT_EQ(mesh.n_faces(), 1);
    EXPECT_EQ(mesh.n_edges(), 3);
    EXPECT_EQ(mesh.n_vertices(), 3);
}

TEST_F(MeshTest, IsTriangleMeshTest) {
    auto v0 = mesh.add_vertex({0.0, 0.0, 0.0});
    auto v1 = mesh.add_vertex({1.0, 0.0, 0.0});
    auto v2 = mesh.add_vertex({0.0, 1.0, 0.0});

    mesh.add_triangle(v0, v1, v2);
    ASSERT_TRUE(mesh.is_triangle_mesh());
}

TEST_F(MeshTest, FindEdgeTest) {
    auto v0 = mesh.add_vertex({0.0, 0.0, 0.0});
    auto v1 = mesh.add_vertex({1.0, 0.0, 0.0});
    auto v2 = mesh.add_vertex({0.0, 1.0, 0.0});

    mesh.add_triangle(v0, v1, v2);
    auto edge01 = mesh.find_edge(v0, v1);
    auto edge12 = mesh.find_edge(v1, v2);
    auto edge20 = mesh.find_edge(v2, v0);

    ASSERT_TRUE(edge01.is_valid());
    ASSERT_TRUE(edge12.is_valid());
    ASSERT_TRUE(edge20.is_valid());
}

TEST_F(MeshTest, IsFlipOkTest) {

    auto v0 = mesh.add_vertex({0.0, 0.0, 0.0});
    auto v1 = mesh.add_vertex({1.0, 0.0, 0.0});
    auto v2 = mesh.add_vertex({0.0, 1.0, 0.0});
    auto v3 = mesh.add_vertex({1.0, 1.0, 0.0});

    auto f = mesh.add_quad(v0, v1, v2, v3);
    mesh.triangulate(f);

    ASSERT_TRUE(mesh.is_flip_ok(mesh.get_edge(mesh.get_halfedge(f))));
}
