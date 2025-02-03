//
// Created by alex on 2/3/25.
//

#include "TreeUtils.h"

namespace Bcg{
    int NodeDepth(const Tree &tree, const Node &n) {
        int depth = 0;
        Node current = n;
        while (!tree.is_root(current)) {
            current = tree.get_parent(current);
            ++depth;
        }
        return depth;
    }

    int SubtreeHeight(const Tree &tree, const Node &n) {
        const std::vector<Node> &child_nodes = tree.get_children(n);
        if (child_nodes.empty()) {
            return 0;
        }
        int max_height = 0;
        for (const Node &child : child_nodes) {
            max_height = std::max(max_height, SubtreeHeight(tree, child));
        }
        return max_height + 1;
    }

    Node LowestCommonAncestor(const Tree &tree, const Node &a, const Node &b) {
        // Get depths for both nodes.
        int depth_a = NodeDepth(tree,a);
        int depth_b = NodeDepth(tree, b);
        Node node_a = a;
        Node node_b = b;
        // Equalize the depth.
        while (depth_a > depth_b) {
            node_a = tree.get_parent(node_a);
            --depth_a;
        }
        while (depth_b > depth_a) {
            node_b = tree.get_parent(node_b);
            --depth_b;
        }
        // Move up until common ancestor is found.
        while (node_a != node_b) {
            node_a = tree.get_parent(node_a);
            node_b = tree.get_parent(node_b);
        }
        return node_a;
    }

    std::vector<Node> PathToRoot(const Tree &tree, const Node &n) {
        std::vector<Node> path;
        Node current = n;
        while (!tree.is_root(current)) {
            path.push_back(current);
            current = tree.get_parent(current);
        }
        path.push_back(tree.root); // include the root
        return path;
    }
    size_t SubtreeSize(const Tree &tree, const Node &n) {
        std::queue<Node> count_queue;
        count_queue.push(n);
        size_t count = 0;
        while (!count_queue.empty()) {
            Node cur = count_queue.front();
            count_queue.pop();
            ++count;
            const std::vector<Node>& children = tree.get_children(cur);
            for (const Node &child : children) {
                count_queue.push(child);
            }
        }
        return count;
    }

    Tree Subtree(const Tree &tree, const Node &n) {
        Tree subtree;

        // First pass: count the number of nodes in the subtree.
        size_t count = SubtreeSize(tree, n);

        // Reserve space in the new tree.
        subtree.reserve(count);

        // Reserve mapping capacity to avoid rehashing.
        std::unordered_map<Node, Node> mapping;
        mapping.reserve(count);

        // Second pass: perform a BFS and copy nodes.
        std::queue<Node> node_queue;
        node_queue.push(n);

        // Create the new root node and map it.
        Node new_root = subtree.new_node();
        mapping[n] = new_root;
        subtree.root = new_root;

        while (!node_queue.empty()) {
            Node current = node_queue.front();
            node_queue.pop();
            Node new_current = mapping[current];

            const std::vector<Node> &child_nodes = tree.get_children(current);
            for (const Node &child : child_nodes) {
                node_queue.push(child);
                Node new_child = subtree.new_node();
                mapping[child] = new_child;
                subtree.attach_to_parent(new_child, new_current);
            }
        }
        return subtree;
    }
}