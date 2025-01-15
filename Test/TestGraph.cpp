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

TEST_F(GraphTest, DFSTraversal) {
    auto A = graph.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
    auto B = graph.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));
    auto C = graph.add_vertex(Vector<Real, 3>(0.0, 0.0, 1.0));
    auto D = graph.add_vertex(Vector<Real, 3>(-1.0, 0.0, 0.0));
    auto E = graph.add_vertex(Vector<Real, 3>(0.0, -1.0, 0.0));

    graph.add_edge(A, B);
    graph.add_edge(A, C);
    graph.add_edge(B, D);
    graph.add_edge(B, E);

    std::vector<Vertex> visited_order;

    graph.dfs_general_with_early_stopping(
        A,
        [&visited_order](const Vertex &v) {
            visited_order.push_back(v); // Record visited vertices.
            return true; // Continue traversal.
        },
        [](const Halfedge &) {
            return true; // Continue traversal for all edges.
        }
    );

    ASSERT_EQ(visited_order.size(), 5);
    EXPECT_EQ(visited_order[0], A);
    EXPECT_EQ(visited_order[1], B);
    EXPECT_EQ(visited_order[2], D);
    EXPECT_EQ(visited_order[3], E);
    EXPECT_EQ(visited_order[4], C);
}

TEST_F(GraphTest, DFSEarlyStopping) {
    auto A = graph.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
    auto B = graph.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));
    auto C = graph.add_vertex(Vector<Real, 3>(0.0, 0.0, 1.0));
    auto D = graph.add_vertex(Vector<Real, 3>(-1.0, 0.0, 0.0));
    auto E = graph.add_vertex(Vector<Real, 3>(0.0, -1.0, 0.0));

    graph.add_edge(A, B);
    graph.add_edge(A, C);
    graph.add_edge(B, D);
    graph.add_edge(B, E);

    std::vector<Vertex> visited_order;

    graph.dfs_general_with_early_stopping(
        A,
        [&visited_order](const Vertex &v) {
            visited_order.push_back(v);
            return v.idx() != 1; // Stop traversal when visiting vertex v2.
        },
        [](const Halfedge &) {
            return true; // Continue traversal for all edges.
        }
    );

    ASSERT_EQ(visited_order.size(), 3);
    EXPECT_EQ(visited_order[0], A);
    EXPECT_EQ(visited_order[1], B);
    EXPECT_EQ(visited_order[2], C);
}

TEST_F(GraphTest, BFSTraversal) {
    auto A = graph.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
    auto B = graph.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));
    auto C = graph.add_vertex(Vector<Real, 3>(0.0, 0.0, 1.0));
    auto D = graph.add_vertex(Vector<Real, 3>(-1.0, 0.0, 0.0));
    auto E = graph.add_vertex(Vector<Real, 3>(0.0, -1.0, 0.0));

    graph.add_edge(A, B);
    graph.add_edge(A, C);
    graph.add_edge(B, D);
    graph.add_edge(B, E);

    std::vector<Vertex> visited_order;

    graph.bfs_general_with_early_stopping(
        A,
        [&visited_order](const Vertex &v) {
            visited_order.push_back(v); // Record visited vertices.
            return true; // Continue traversal.
        },
        [](const Halfedge &) {
            return true; // Continue traversal for all edges.
        }
    );

    EXPECT_EQ(visited_order.size(), 5);
    // Check the BFS traversal order.
    EXPECT_EQ(visited_order[0], A);
    EXPECT_TRUE(
            (visited_order[1] == B && visited_order[2] == C) ||
            (visited_order[2] == B && visited_order[1] == C)
    );
    EXPECT_TRUE(
            (visited_order[3] == D && visited_order[4] == E) ||
            (visited_order[3] == E && visited_order[4] == D)
    );
}


TEST_F(GraphTest, BFSEarlyStopping) {
    auto A = graph.add_vertex(Vector<Real, 3>(1.0, 0.0, 0.0));
    auto B = graph.add_vertex(Vector<Real, 3>(0.0, 1.0, 0.0));
    auto C = graph.add_vertex(Vector<Real, 3>(0.0, 0.0, 1.0));
    auto D = graph.add_vertex(Vector<Real, 3>(-1.0, 0.0, 0.0));
    auto E = graph.add_vertex(Vector<Real, 3>(0.0, -1.0, 0.0));

    graph.add_edge(A, B);
    graph.add_edge(A, C);
    graph.add_edge(B, D);
    graph.add_edge(B, E);

    std::vector<Vertex> visited_order;

    graph.bfs_general_with_early_stopping(
        A,
        [&visited_order](const Vertex &v) {
            visited_order.push_back(v);
            return v.idx() <= 3; // Stop traversal when visiting vertex v3.
        },
        [](const Halfedge &) {
            return true; // Continue traversal for all edges.
        }
    );

    EXPECT_EQ(visited_order.size(), 4);
    EXPECT_EQ(visited_order[0], A);
    EXPECT_TRUE(
            (visited_order[1] == B && visited_order[2] == C) ||
            (visited_order[2] == B && visited_order[1] == C)
    );
    EXPECT_TRUE((visited_order[3] == D || visited_order[3] == E));
}
