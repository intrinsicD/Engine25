//
// Created by alex on 2/3/25.
//

#ifndef TREEUTILS_H
#define TREEUTILS_H

#include "Tree.h"

namespace Bcg {
    /**
     * @brief Calculates the depth of a node in the tree.
     * @param tree The tree containing the node.
     * @param n The node whose depth is to be calculated.
     * @return The depth of the node.
     */
    int NodeDepth(const Tree &tree, const Node &n);

    /**
     * @brief Calculates the height of the subtree rooted at a given node.
     * @param tree The tree containing the node.
     * @param n The root node of the subtree.
     * @return The height of the subtree.
     */
    int SubtreeHeight(const Tree &tree, const Node &n);

    /**
     * @brief Finds the lowest common ancestor of two nodes in the tree.
     * @param tree The tree containing the nodes.
     * @param a The first node.
     * @param b The second node.
     * @return The lowest common ancestor of the two nodes.
     */
    Node LowestCommonAncestor(const Tree &tree, const Node &a, const Node &b);

    /**
     * @brief Finds the path from a node to the root of the tree.
     * @param tree The tree containing the node.
     * @param n The node from which the path is to be found.
     * @return A vector of nodes representing the path from the node to the root.
     */
    std::vector<Node> PathToRoot(const Tree &tree, const Node &n);

    /**
     * @brief Calculates the size of the subtree rooted at a given node.
     * @param tree The tree containing the node.
     * @param n The root node of the subtree.
     * @return The size of the subtree.
     */
    size_t SubtreeSize(const Tree &tree, const Node &n);

    /**
     * @brief Creates a subtree from a given node.
     * @param tree The tree containing the node.
     * @param n The root node of the subtree.
     * @return A new tree representing the subtree.
     */
    Tree Subtree(const Tree &tree, const Node &n);
}

#endif //TREEUTILS_H
