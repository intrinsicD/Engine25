//
// Created by alex on 2/3/25.
//

// TreeTest.cpp
#include "TreeUtils.h"
#include "gtest/gtest.h"

using namespace Bcg;

// Helper function to build a sample tree:
// Structure:
//
//            root (0)
//           /       \
//       child1 (1)   child2 (2)
//         /    \           \
//   child3 (3) child4 (4)  child5 (5)
//
static Node BuildSampleTree(Tree &tree,
                            Node &child1,
                            Node &child2,
                            Node &child3,
                            Node &child4,
                            Node &child5) {
    // Create nodes. Assume new_node() returns a Node that uniquely identifies
    // the node (e.g. via an index).

    tree.root = tree.new_node(); // node 0
    child1 = tree.new_node(); // node 1
    child2 = tree.new_node(); // node 2
    child3 = tree.new_node(); // node 3
    child4 = tree.new_node(); // node 4
    child5 = tree.new_node(); // node 5

    // Build the tree structure.
    // Attach child1 and child2 to root.
    tree.attach_to_parent(child1, tree.root);
    tree.attach_to_parent(child2, tree.root);
    // Attach child3 and child4 to child1.
    tree.attach_to_parent(child3, child1);
    tree.attach_to_parent(child4, child1);
    // Attach child5 to child2.
    tree.attach_to_parent(child5, child2);
    return tree.root;
}

TEST(TreeTest, BasicStructure) {
    Tree tree;
    Node child1, child2, child3, child4, child5;
    Node root = BuildSampleTree(tree, child1, child2, child3, child4, child5);

    // Verify the total number of nodes.
    EXPECT_EQ(tree.n_nodes(), 6);

    // Verify children of the root.
    const std::vector<Node> &root_children = tree.get_children(root);
    ASSERT_EQ(root_children.size(), 2);
    EXPECT_EQ(root_children[0], child1);
    EXPECT_EQ(root_children[1], child2);

    // Verify children of child1.
    const std::vector<Node> &child1_children = tree.get_children(child1);
    ASSERT_EQ(child1_children.size(), 2);
    EXPECT_EQ(child1_children[0], child3);
    EXPECT_EQ(child1_children[1], child4);

    // Verify children of child2.
    const std::vector<Node> &child2_children = tree.get_children(child2);
    ASSERT_EQ(child2_children.size(), 1);
    EXPECT_EQ(child2_children[0], child5);
}

TEST(TreeTest, DFSPreorderTraversal) {
    Tree tree;
    Node child1, child2, child3, child4, child5;
    Node root = BuildSampleTree(tree, child1, child2, child3, child4, child5);


    // Expected DFS (pre-order) traversal:
    // Pre-order: Visit the node, then its children (left-to-right).
    // With the implementation (pushing children in reverse order so that the
    // leftmost child is processed first), we expect:
    // root, child1, child3, child4, child2, child5.
    std::vector<Node> expected_order = {root, child1, child3, child4, child2, child5};

    std::vector<Node> dfs_order;
    // Use the DFS iterator starting at the root.
    for (auto n: tree.dfs(root)) {
        dfs_order.push_back(n);
    }

    ASSERT_EQ(dfs_order.size(), expected_order.size());
    for (size_t i = 0; i < expected_order.size(); ++i) {
        EXPECT_EQ(dfs_order[i], expected_order[i])
            << "DFS order mismatch at index " << i;
    }
}

TEST(TreeTest, BFSLevelOrderTraversal) {
    Tree tree;
    Node child1, child2, child3, child4, child5;
    Node root = BuildSampleTree(tree, child1, child2, child3, child4, child5);


    // Expected BFS (level-order) traversal:
    // Level-order: root, then its children, then grandchildren, etc.
    // Expected order: root, child1, child2, child3, child4, child5.
    std::vector<Node> expected_order = {root, child1, child2, child3, child4, child5};

    std::vector<Node> bfs_order;
    // Use the BFS iterator starting at the root.
    for (auto n: tree.bfs(root)) {
        bfs_order.push_back(n);
    }

    ASSERT_EQ(bfs_order.size(), expected_order.size());
    for (size_t i = 0; i < expected_order.size(); ++i) {
        EXPECT_EQ(bfs_order[i], expected_order[i])
            << "BFS order mismatch at index " << i;
    }
}


TEST(GarbageCollectionTest, RemoveDeletedNodes) {
    Tree tree;
    Node child1, child2, child3, child4, child5;
    Node root = BuildSampleTree(tree, child1, child2, child3, child4, child5);

    auto named = tree.node_property<int>("n:name");
    named[root] = 0;
    named[child1] = 1;
    named[child2] = 2;
    named[child3] = 3;
    named[child4] = 4;
    named[child5] = 5;

    // Mark two nodes as deleted.
    tree.delete_node(child3);
    tree.delete_node(child5);

    // Verify that the deletion counter indicates 2 deleted nodes.
    EXPECT_EQ(tree.nodes.num_deleted, 2);

    // Run garbage collection.
    tree.garbage_collection();

    // After garbage collection, the total number of nodes should be reduced.
    // In our sample tree, we started with 6 nodes and marked 2 as deleted.
    EXPECT_EQ(tree.n_nodes(), 4u);

    // Verify that no remaining node is marked as deleted.
    for (auto n : tree.dfs(root)) {

        EXPECT_FALSE(tree.is_deleted(n));
        // For non-root nodes, the parent should be valid.
        if (!tree.is_root(n)) {
            EXPECT_TRUE(tree.get_parent(n).is_valid());
        }
        // The children list should only contain valid nodes.
        for (const Node &child : tree.get_children(n)) {
            EXPECT_TRUE(child.is_valid());
        }
    }

    // Verify that the deleted nodes (child3 and child5) no longer appear in the traversal.
    std::vector<Node> dfs_nodes;
    for (auto n : tree.dfs(root)) {
        dfs_nodes.push_back(n);
    }

    auto contains = [&](int name_value) -> bool {
        for (const Node &n : dfs_nodes) {
            if (named[n] == name_value) {
                return true;
            }
        }
        return false;
    };

    EXPECT_FALSE(contains(3));
    EXPECT_FALSE(contains(5));

    // Finally, ensure that the deletion counter is reset.
    EXPECT_EQ(tree.nodes.num_deleted, 0);
}

TEST(TreeUtilsTest, NodeDepthTest) {
    Tree tree;
    Node child1, child2, child3, child4, child5;
    Node root = BuildSampleTree(tree, child1, child2, child3, child4, child5);


    EXPECT_EQ(NodeDepth(tree, root), 0);
    EXPECT_EQ(NodeDepth(tree, child1), 1);
    EXPECT_EQ(NodeDepth(tree, child2), 1);
    EXPECT_EQ(NodeDepth(tree, child3), 2);
    EXPECT_EQ(NodeDepth(tree, child4), 2);
    EXPECT_EQ(NodeDepth(tree, child5), 2);
}

TEST(TreeUtilsTest, SubtreeHeightTest) {
    Tree tree;
    Node child1, child2, child3, child4, child5;
    Node root = BuildSampleTree(tree, child1, child2, child3, child4, child5);


    // Height: maximum number of edges from the node to a leaf.
    // For our tree:
    //   - root has height 2,
    //   - child1 and child2 have height 1,
    //   - child3, child4, and child5 are leaves (height 0).
    EXPECT_EQ(SubtreeHeight(tree, root), 2);
    EXPECT_EQ(SubtreeHeight(tree, child1), 1);
    EXPECT_EQ(SubtreeHeight(tree, child2), 1);
    EXPECT_EQ(SubtreeHeight(tree, child3), 0);
    EXPECT_EQ(SubtreeHeight(tree, child4), 0);
    EXPECT_EQ(SubtreeHeight(tree, child5), 0);
}

TEST(TreeUtilsTest, LowestCommonAncestorTest) {
    Tree tree;
    Node child1, child2, child3, child4, child5;
    Node root = BuildSampleTree(tree, child1, child2, child3, child4, child5);


    // LCA(child3, child4) should be child1.
    EXPECT_EQ(LowestCommonAncestor(tree, child3, child4), child1);

    // LCA(child3, child2) should be root.
    EXPECT_EQ(LowestCommonAncestor(tree, child3, child2), root);

    // LCA(child3, child5) should also be root.
    EXPECT_EQ(LowestCommonAncestor(tree, child3, child5), root);
}

TEST(TreeUtilsTest, PathToRootTest) {
    Tree tree;
    Node child1, child2, child3, child4, child5;
    Node root = BuildSampleTree(tree, child1, child2, child3, child4, child5);


    // For child3, the expected path to the root is: child3, child1, root.
    std::vector<Node> path = PathToRoot(tree, child3);
    ASSERT_EQ(path.size(), 3u);
    EXPECT_EQ(path[0], child3);
    EXPECT_EQ(path[1], child1);
    EXPECT_EQ(path[2], root);
}

TEST(TreeUtilsTest, SubtreeSizeTest) {
    Tree tree;
    Node child1, child2, child3, child4, child5;
    Node root = BuildSampleTree(tree, child1, child2, child3, child4, child5);


    // Subtree sizes:
    //   - Root: 6 nodes (0, 1, 2, 3, 4, 5)
    //   - child1: 3 nodes (child1, child3, child4)
    //   - child2: 2 nodes (child2, child5)
    //   - child3: 1 node
    EXPECT_EQ(SubtreeSize(tree, root), 6u);
    EXPECT_EQ(SubtreeSize(tree, child1), 3u);
    EXPECT_EQ(SubtreeSize(tree, child2), 2u);
    EXPECT_EQ(SubtreeSize(tree, child3), 1u);
}

TEST(TreeUtilsTest, SubtreeExtractionTest) {
    Tree tree;
    Node child1, child2, child3, child4, child5;
    Node root = BuildSampleTree(tree, child1, child2, child3, child4, child5);


    // Extract the subtree rooted at child1.
    Tree subtree = Subtree(tree, child1);

    // The extracted subtree should contain the nodes corresponding to child1, child3, and child4.
    // So its subtree size should be 3.
    EXPECT_EQ(SubtreeSize(subtree, subtree.root), 3u);

    // Verify structure: The new subtree's root should have two children.
    const std::vector<Node> &sub_children = subtree.get_children(subtree.root);
    ASSERT_EQ(sub_children.size(), 2u);

    // Both children in the extracted subtree should be leaves.
    EXPECT_EQ(SubtreeSize(subtree, sub_children[0]), 1u);
    EXPECT_EQ(SubtreeSize(subtree, sub_children[1]), 1u);
}

