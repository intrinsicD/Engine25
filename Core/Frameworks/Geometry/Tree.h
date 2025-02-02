//
// Created by alex on 2/2/25.
//

#ifndef TREE_H
#define TREE_H

#include "Macros.h"
#include "GeometricProperties.h"

namespace Bcg{
  class Tree {
    public:
        NodeProperty<bool> n_deleted; ///< Deletion flag for nodes.
        NodeProperty<Node> parents; ///< Deletion flag for nodes.
        NodeProperty<std::vector<Node>> children; ///< Deletion flag for nodes.
        Nodes nodes; ///< Container for managing nodes data.

        /**
         * @brief Default constructor. Initializes properties and containers.
         */
        Tree();

        /**
         * @brief Virtual destructor for extensibility.
         */
        virtual ~Tree() = default;

        /**
         * @brief Copy constructor.
         * @param rhs Another Tree instance to copy.
         */
        Tree(const Tree &rhs) { operator=(rhs); }

        /**
         * @brief Copy assignment operator.
         * @param rhs Another Tree instance to copy.
         * @return Reference to this Tree.
         */
        Tree &operator=(const Tree &rhs);

        /**
         * @brief Deep copy assignment method.
         * @param rhs Another Tree instance to copy.
         * @return Reference to this Tree.
         */
        Tree &assign(const Tree &rhs);

        /**
         * @brief Clears all nodes and properties.
         */
        virtual void clear();

        /**
         * @brief Releases unused memory.
         */
        virtual void free_memory();

        /**
         * @brief Reserves space for a specified number of nodes.
         * @param nnodes Number of nodes to reserve space for.
         */
        virtual void reserve(size_t nnodes);

        /**
         * @brief Removes deleted nodes and compacts the data.
         */
        virtual void garbage_collection();

        // -------------------------------------------------------------------------------------------------------------

        // Node Methods
        // -------------------------------------------------------------------------------------------------------------

        /**
         * @brief Returns the total number of undeleted nodes.
         * @return Number of undeleted nodes.
         */
        [[nodiscard]] size_t n_nodes() const { return nodes.n_nodes(); }

        /**
         * @brief Adds a node to the tree.
         * @return The newly created node.
         */
        Node new_node();

        /**
         * @brief Marks a node as deleted.
         * @param n Node to mark as deleted.
         */
        void mark_deleted(const Node &n);

        /**
         * @brief Deletes a node.
         * @param n Node to delete.
         */
        virtual void delete_node(const Node &n);

        /**
         * @brief Sets the parent of a given node.
         * @param n The node for which to set the parent.
         * @param parent The parent node to set.
         */
        void set_parent(const Node &n, const Node &parent){
            parents[n] = parent;
        }

        /**
         * @brief Gets the parent of a given node.
         * @param n The node for which to get the parent.
         * @return The parent node.
         */
        Node get_parent(const Node &n) const{
            return parents[n];
        }

        /**
         * @brief Sets the children of a given node.
         * @param n The node for which to set the children.
         * @param children A vector of child nodes to set.
         */
        void set_children(const Node &n, const std::vector<Node> &child_nodes){
          for (const auto &child : child_nodes) {
              set_orphan(child);
          }

           children[n] = child_nodes;
        }

        /**
         * @brief Gets the children of a given node.
         * @param n The node for which to get the children.
         * @return A vector of child nodes.
         */
        std::vector<Node> get_children(const Node &n) const;

        /**
         * @brief Adds a child to a given node.
         * @param n The node to which to add the child.
         * @param child The child node to add.
         * @return True if the child was added successfully, false otherwise.
         */
        bool add_child(const Node &n, const Node &child);

        /**
         * @brief Removes a child from a given node.
         * @param n The node from which to remove the child.
         * @param child The child node to remove.
         * @return True if the child was removed successfully, false otherwise.
         */
        bool remove_child(const Node &n, const Node &child);

        /**
         * @brief Checks if a given node is a leaf node.
         * @param n The node to check.
         * @return True if the node is a leaf, false otherwise.
         */
        bool is_leaf(const Node &n) const;

        /**
         * @brief Checks if a given node is the root node.
         * @param n The node to check.
         * @return True if the node is the root, false otherwise.
         */
        bool is_root(const Node &n) const;

        /**
         * @brief Checks if a given node is valid.
         * @param n The node to check.
         * @return True if the node is valid, false otherwise.
         */
        bool is_valid(const Node &n) const;

        /**
         * @brief Checks if a given node is marked as deleted.
         * @param n The node to check.
         * @return True if the node is marked as deleted, false otherwise.
         */
        bool is_deleted(const Node &n) const;

        /**
         * @brief Checks if there are any deleted nodes that need to be garbage collected.
         * @return True if there are deleted nodes, false otherwise.
         */
        bool has_garbage() const;

        /**
         * @brief Finds the index of a child node within the children of a parent node.
         * @param parent The parent node.
         * @param child The child node to find.
         * @return The index of the child node if found, otherwise -1.
         */
        size_t find_child(const Node &parent, const Node &child) const{
            const std::vector<Node> &child_nodes = get_children(parent);
            for (size_t i = 0; i < child_nodes.size(); ++i) {
                if (child_nodes[i] == child) {
                    return i;
                }
            }
            return BCG_INVALID_INDEX;
        }

       /**
         * @brief Sets the parent of a given node to an orphan (no parent).
         * @param n The node to set as an orphan.
         */
        void set_orphan(const Node &n){
          Node parent = parents[n];
          size_t idx = find_child(parent, n);
          if (idx != BCG_INVALID_INDEX) {
              std::vector<Node> &child_nodes = children[parent];
              std::swap(child_nodes[idx], child_nodes.back());
              child_nodes.pop_back();
          }
          parents[n] = Node(BCG_INVALID_INDEX);
        }
  };

}

#endif //TREE_H
