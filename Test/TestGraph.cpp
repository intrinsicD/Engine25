//
// Created by alex on 10.01.25.
//

// TestGraph.cpp
#include "TestGraph.h"

TEST_F(GraphTest, DefaultConstructor) {
    EXPECT_TRUE(graph.is_empty());
    EXPECT_FALSE(graph.has_garbage());
    EXPECT_TRUE(graph.positions);
}

TEST_F(GraphTest, AddVertex) {
    // Create Graph
    Create();
    EXPECT_TRUE(A.is_valid());
    EXPECT_EQ(graph.n_vertices(), 5);
    EXPECT_EQ(graph.positions[A], (Vector<Real, 3>(1.0, 0.0, 0.0)));
}

TEST_F(GraphTest, AddAndDeleteEdge) {
    Create();

    EXPECT_TRUE(AB.is_valid());
    EXPECT_EQ(graph.n_edges(), 4);

    graph.delete_edge(graph.get_edge(AB));
    EXPECT_EQ(graph.n_edges(), 3);
    EXPECT_TRUE(graph.has_garbage());
}

TEST_F(GraphTest, GarbageCollection) {
    Create();

    graph.mark_deleted(A);
    graph.mark_deleted(D);

    EXPECT_TRUE(graph.has_garbage());

    graph.garbage_collection();
    EXPECT_FALSE(graph.has_garbage());
    EXPECT_EQ(graph.n_vertices(), 3);
    EXPECT_EQ(graph.positions[Vertex(0)], (Vector<Real, 3>(0.0, -1.0, 0.0)));
}

TEST_F(GraphTest, ConnectivityValidation) {
    Create();

    EXPECT_EQ(graph.get_valence(A), 2);
    EXPECT_EQ(graph.get_valence(B), 3);
    EXPECT_EQ(graph.get_valence(C), 1);
    EXPECT_EQ(graph.get_valence(D), 1);
    EXPECT_EQ(graph.get_valence(E), 1);

    // Check if the edges are as expected
    auto edges = Edges(graph);
    EXPECT_EQ(graph.edges.size(), 4);
    EXPECT_EQ(edges[Edge(0)], (Vector<unsigned int, 2>(1, 0)));
    EXPECT_EQ(edges[Edge(1)], (Vector<unsigned int, 2>(2, 0)));
}

TEST_F(GraphTest, DFSRangeLoopIterator) {
    // Create Graph
    Create();

    std::vector<Vertex> visited_order;
    // Use the DFS range-based iterator
    for (const Vertex &v: graph.dfs(A)) {
        visited_order.push_back(v);
    }

    ASSERT_EQ(visited_order.size(), 5);
    // Expected DFS order: A, B, D, E, C.
    EXPECT_EQ(visited_order[0], A);
    EXPECT_EQ(visited_order[1], B);
    EXPECT_EQ(visited_order[2], D);
    EXPECT_EQ(visited_order[3], E);
    EXPECT_EQ(visited_order[4], C);
}

TEST_F(GraphTest, BFSRangeLoopIterator) {
    // Create Graph
    Create();

    std::vector<Vertex> visited_order;
    // Use the BFS range-based iterator
    for (const Vertex &v: graph.bfs(A)) {
        visited_order.push_back(v);
    }

    ASSERT_EQ(visited_order.size(), 5);
    // Expected BFS order:
    //   - The first vertex is A.
    //   - The next two vertices are B and C (order can vary).
    //   - The last two vertices are D and E (order can vary).
    EXPECT_EQ(visited_order[0], A);
    EXPECT_TRUE(
        (visited_order[1] == B && visited_order[2] == C) ||
        (visited_order[1] == C && visited_order[2] == B)
    );
    EXPECT_TRUE(
        (visited_order[3] == D && visited_order[4] == E) ||
        (visited_order[3] == E && visited_order[4] == D)
    );
}
