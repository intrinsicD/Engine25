//
// Created by alex on 10.01.25.
//

// TestGraph.cpp
#include "gtest/gtest.h"
#include "Graph.h"

using namespace Bcg;

// Test fixture for Graph
class GraphTest : public ::testing::Test {
protected:
    Graph graph;
};

TEST_F(GraphTest, DefaultConstructor) {
    EXPECT_TRUE(graph.is_empty());
    EXPECT_FALSE(graph.has_garbage());
    EXPECT_TRUE(graph.positions);
}

TEST_F(GraphTest, AddVertex) {
    auto vertex = graph.add_vertex(Vector<Real, 3>(1.0, 2.0, 3.0));
    EXPECT_TRUE(vertex.is_valid());
    EXPECT_EQ(graph.n_vertices(), 1);
    EXPECT_EQ(graph.positions[vertex], (Vector<Real, 3>(1.0, 2.0, 3.0)));
}

TEST_F(GraphTest, AddAndDeleteEdge) {
    auto v1 = graph.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
    auto v2 = graph.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));

    auto edge = graph.add_edge(v1, v2);
    EXPECT_TRUE(edge.is_valid());
    EXPECT_EQ(graph.n_edges(), 1);

    graph.delete_edge(graph.get_edge(edge));
    EXPECT_EQ(graph.n_edges(), 0);
    EXPECT_TRUE(graph.has_garbage());
}

TEST_F(GraphTest, GarbageCollection) {
    auto v1 = graph.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
    auto v2 = graph.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));
    auto v3 = graph.add_vertex(Vector<Real, 3>(0.0, 0.0, 1.0));

    graph.mark_deleted(v1);
    graph.mark_deleted(v2);

    EXPECT_TRUE(graph.has_garbage());

    graph.garbage_collection();
    EXPECT_FALSE(graph.has_garbage());
    EXPECT_EQ(graph.n_vertices(), 1);
    EXPECT_EQ(graph.positions[Vertex(0)], (Vector<Real, 3>(0.0, 0.0, 1.0)));
}

TEST_F(GraphTest, ConnectivityValidation) {
    auto v1 = graph.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
    auto v2 = graph.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));
    auto v3 = graph.add_vertex(Vector<Real, 3>(0.0, 0.0, 1.0));

    graph.add_edge(v1, v2);
    graph.add_edge(v2, v3);

    EXPECT_EQ(graph.get_valence(v2), 2);
    EXPECT_EQ(graph.get_valence(v1), 1);
    EXPECT_EQ(graph.get_valence(v3), 1);

    // Check if the edges are as expected
    auto edges = graph.get_edges();
    EXPECT_EQ(graph.edges.size(), 2);
    EXPECT_EQ(edges[Edge(0)], (Vector<unsigned int, 2>(1, 0)));
    EXPECT_EQ(edges[Edge(1)], (Vector<unsigned int, 2>(2, 1)));
}

TEST_F(GraphTest, EdgeProperty) {
    auto v1 = graph.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
    auto v2 = graph.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));
    auto v3 = graph.add_vertex(Vector<Real, 3>(0.0, 0.0, 1.0));

    graph.add_edge(v1, v2);
    graph.add_edge(v2, v3);

    // Check if the edges are as expected
    auto edges = graph.get_edges();
    EXPECT_EQ(graph.edges.size(), 2);
    EXPECT_EQ(edges[Edge(0)], (Vector<unsigned int, 2>(1, 0)));
    EXPECT_EQ(edges[Edge(1)], (Vector<unsigned int, 2>(2, 1)));
}
