//
// Created by alex on 2/2/25.
//

#ifndef TREE_H
#define TREE_H

#include "GeometricProperties.h"
#include <stack>
#include <queue>

namespace Bcg {
    template<class TreeContainer>
    class DFSIterator {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Node;
        using reference = const Node &;
        using pointer = const Node *;
        using iterator_category = std::forward_iterator_tag;

        // Construct iterator starting at 'root'
        DFSIterator(const TreeContainer *tree, const Node &root) : m_tree(tree) {
            if (m_tree && m_tree->is_valid(root)) {
                m_node_stack.push(root);
            }
        }

        // End iterator
        DFSIterator() : m_tree(nullptr) {
        }

        reference operator*() const {
            return m_node_stack.top();
        }

        pointer operator->() const {
            return &m_node_stack.top();
        }

        DFSIterator &operator++() {
            if (!m_node_stack.empty()) {
                Node current = m_node_stack.top();
                m_node_stack.pop();
                const std::vector<Node> &child_nodes = m_tree->get_children(current);
                // Push children in reverse order so the leftmost is processed first.
                for (auto it = child_nodes.rbegin(); it != child_nodes.rend(); ++it) {
                    if (m_tree->is_valid(*it)) {
                        m_node_stack.push(*it);
                    }
                }
            }
            if (m_node_stack.empty()) {
                m_tree = nullptr; // Mark as end.
            }
            return *this;
        }

        DFSIterator operator++(int) {
            DFSIterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const DFSIterator &other) const {
            // End iterators are identified by m_tree being nullptr.
            if (m_tree == nullptr && other.m_tree == nullptr) {
                return true;
            }
            // Compare both the tree pointer and the underlying stack.
            return m_tree == other.m_tree && m_node_stack == other.m_node_stack;
        }

        bool operator!=(const DFSIterator &other) const {
            return !(*this == other);
        }

    private:
        const TreeContainer *m_tree;
        std::stack<Node, std::vector<Node> > m_node_stack;
    };

    template<class TreeContainer>
    class BFSIterator {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Node;
        using reference = const Node &;
        using pointer = const Node *;
        using iterator_category = std::forward_iterator_tag;

        BFSIterator(const TreeContainer *tree, const Node &root) : m_tree(tree) {
            if (m_tree && m_tree->is_valid(root)) {
                m_node_queue.push(root);
            }
        }

        BFSIterator() : m_tree(nullptr) {
        }

        reference operator*() const {
            return m_node_queue.front();
        }

        pointer operator->() const {
            return &m_node_queue.front();
        }

        BFSIterator &operator++() {
            if (!m_node_queue.empty()) {
                Node current = m_node_queue.front();
                m_node_queue.pop();
                const std::vector<Node> &child_nodes = m_tree->get_children(current);
                for (const auto &child: child_nodes) {
                    if (m_tree->is_valid(child)) {
                        m_node_queue.push(child);
                    }
                }
            }
            if (m_node_queue.empty()) {
                m_tree = nullptr;
            }
            return *this;
        }

        BFSIterator operator++(int) {
            BFSIterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const BFSIterator &other) const {
            // For simplicity, consider iterators equal if both are at end.
            if (m_tree == nullptr && other.m_tree == nullptr) {
                return true;
            }

            // If not at the end, we compare m_tree and front element.
            return m_tree == other.m_tree &&
                   (!m_node_queue.empty() && !other.m_node_queue.empty() &&
                    m_node_queue.front() == other.m_node_queue.front());
        }

        bool operator!=(const BFSIterator &other) const {
            return !(*this == other);
        }

    private:
        const TreeContainer *m_tree;
        std::queue<Node> m_node_queue;
    };

    // Range wrapper for DFS (pre-order) traversal.
    template<class TreeContainer>
    class DFSRange {
    public:
        DFSRange(const TreeContainer *tree, const Node &root)
            : tree_(tree), m_root(root) {
        }

        // Returns the beginning iterator.
        DFSIterator<TreeContainer> begin() const {
            return DFSIterator<TreeContainer>(tree_, m_root);
        }

        // Returns the end iterator (default-constructed).
        DFSIterator<TreeContainer> end() const {
            return DFSIterator<TreeContainer>();
        }

    private:
        const TreeContainer *tree_;
        Node m_root;
    };

    // Range wrapper for BFS (level-order) traversal.
    template<class TreeContainer>
    class BFSRange {
    public:
        BFSRange(const TreeContainer *tree, const Node &root)
            : tree_(tree), m_root(root) {
        }

        // Returns the beginning iterator.
        BFSIterator<TreeContainer> begin() const {
            return BFSIterator<TreeContainer>(tree_, m_root);
        }

        // Returns the end iterator.
        BFSIterator<TreeContainer> end() const {
            return BFSIterator<TreeContainer>();
        }

    private:
        const TreeContainer *tree_;
        Node m_root;
    };

    class Tree {
    public:
        NodeProperty<bool> n_deleted; ///< Deletion flag for nodes.
        NodeProperty<Node> parents; ///< Parent node for each node.
        NodeProperty<std::vector<Node> > children; ///< Child nodes for each node.
        Nodes nodes; ///< Container for managing nodes data.
        Node root; ///< Root node of the tree.

        /**
         * @brief Default constructor for the Tree class.
         */
        Tree();

        /**
         * @brief Virtual destructor for the Tree class.
         */
        virtual ~Tree() = default;

        /**
         * @brief Copy constructor for the Tree class.
         * @param rhs The Tree object to copy from.
         */
        Tree(const Tree &rhs) { operator=(rhs); }

        /**
         * @brief Copy assignment operator for the Tree class.
         * @param rhs The Tree object to assign from.
         * @return A reference to the assigned Tree object.
         */
        Tree &operator=(const Tree &rhs);

        /**
         * @brief Assigns the contents of another Tree object to this Tree.
         * @param rhs The Tree object to assign from.
         * @return A reference to the assigned Tree object.
         */
        Tree &assign(const Tree &rhs);

        /**
         * @brief Clears the contents of the Tree.
         */
        virtual void clear();

        /**
         * @brief Frees the memory used by the Tree.
         */
        virtual void free_memory();

        /**
         * @brief Reserves memory for a specified number of nodes.
         * @param nnodes The number of nodes to reserve memory for.
         */
        virtual void reserve(size_t nnodes);

        /**
         * @brief Performs garbage collection to remove deleted nodes.
         */
        virtual void garbage_collection();

        // --------------------------------------------------------------------
        // Node Methods (public interface)
        // --------------------------------------------------------------------
        /**
         * @brief Returns the number of nodes in the tree.
         * @return The number of nodes.
         */
        [[nodiscard]] size_t n_nodes() const { return nodes.n_nodes(); }

        /**
         * @brief Checks if the tree is empty.
         * @return True if the tree is empty, false otherwise.
         */
        [[nodiscard]] bool is_empty() const { return n_nodes() == 0; }

        /**
         * @brief Checks if a node is a leaf (i.e., has no children).
         * @param n The node to check.
         * @return True if the node is a leaf, false otherwise.
         */
        [[nodiscard]] bool is_leaf(const Node &n) const { return get_children(n).empty(); }

        /**
         * @brief Checks if a node is the root of the tree.
         * @param n The node to check.
         * @return True if the node is the root, false otherwise.
         */
        [[nodiscard]] bool is_root(const Node &n) const { return n == root; }

        /**
         * @brief Checks if a node is an orphan (i.e., has no parent).
         * @param n The node to check.
         * @return True if the node is an orphan, false otherwise.
         */
        [[nodiscard]] bool is_orphan(const Node &n) const { return parents[n] == Node(); }

        /**
         * @brief Checks if a node is valid (i.e., exists in the tree).
         * @param n The node to check.
         * @return True if the node is valid, false otherwise.
         */
        [[nodiscard]] bool is_valid(const Node &n) const { return nodes.is_valid(n); }

        /**
         * @brief Checks if a node is marked as deleted.
         * @param n The node to check.
         * @return True if the node is marked as deleted, false otherwise.
         */
        [[nodiscard]] bool is_deleted(const Node &n) const { return n_deleted[n]; }

        /**
         * @brief Checks if the tree has any garbage nodes.
         * @return True if the tree has garbage nodes, false otherwise.
         */
        [[nodiscard]] bool has_garbage() const { return nodes.has_garbage(); }
        /**
         * @brief Adds a new node property to the tree.
         * @tparam T The type of the property.
         * @param name The name of the property.
         * @param t The default value of the property.
         * @return A NodeProperty object representing the added property.
         */
        template<class T>
        NodeProperty<T> add_node_property(const std::string &name, const T t = T()) {
            return NodeProperty<T>(nodes.add<T>(name, t));
        }

        /**
         * @brief Retrieves a node property by name.
         * @tparam T The type of the property.
         * @param name The name of the property.
         * @return A NodeProperty object representing the retrieved property.
         */
        template<class T>
        NodeProperty<T> get_node_property(const std::string &name) const {
            return NodeProperty<T>(nodes.get<T>(name));
        }

        /**
         * @brief Retrieves or adds a node property by name.
         * @tparam T The type of the property.
         * @param name The name of the property.
         * @param t The default value of the property if it needs to be added.
         * @return A NodeProperty object representing the retrieved or added property.
         */
        template<class T>
        NodeProperty<T> node_property(const std::string &name, const T t = T()) {
            return NodeProperty<T>(nodes.get_or_add<T>(name, t));
        }

        /**
         * @brief Removes a node property.
         * @tparam T The type of the property.
         * @param p The NodeProperty object representing the property to be removed.
         */
        template<class T>
        void remove_node_property(NodeProperty<T> &p) {
            nodes.remove(p);
        }

        /**
         * @brief Checks if a node property exists by name.
         * @param name The name of the property.
         * @return True if the property exists, false otherwise.
         */
        [[nodiscard]] bool has_node_property(const std::string &name) const {
            return nodes.exists(name);
        }

        /**
         * @brief Creates a new node in the tree.
         * @return The newly created node.
         */
        Node new_node() {
            nodes.push_back();
            return Node(nodes.size() - 1);
        }

        /**
         * @brief Marks a node as deleted.
         * @param n The node to mark as deleted.
         */
        void mark_deleted(const Node &n);

        /**
         * @brief Deletes a node and its children from the tree.
         * @param n The node to delete.
         */
        virtual void delete_node(const Node &n);

        /**
         * @brief Attaches a node to a parent node.
         * If the node is already attached (i.e. non-orphan), it will be detached first.
         * @param n The node to attach.
         * @param parent The parent node to attach to.
         */
        void attach_to_parent(const Node &n, const Node &parent);

        /**
         * @brief Attaches an orphan node to a parent node.
         * @pre The node 'orphan' must be an orphan.
         * @param orphan The orphan node to attach.
         * @param parent The parent node to attach to.
         */
        void attach_orphan_to_parent(const Node &orphan, const Node &parent) {
            assert(is_orphan(orphan));
            parents[orphan] = parent;
            children[parent].push_back(orphan);
        }

        /**
         * @brief Attaches multiple orphan nodes to a parent node.
         * @pre Each node in 'orphans' must be an orphan.
         * @param orphans A vector of orphan nodes to attach.
         * @param new_parent The parent node to attach to.
         */
        void attach_orphans_to_parent(const std::vector<Node> &orphans, const Node &new_parent);

        /**
         * @brief Detaches a node from its parent and marks it as an orphan.
         * @param n The node to detach.
         */
        void detach_from_parent(const Node &n);

        /**
         * @brief Detaches multiple nodes from their respective parents.
         * @param nodes_to_detach A vector of nodes to detach.
         */
        void detach_from_parents(const std::vector<Node> &nodes_to_detach);

        /**
         * @brief Detaches all children from a given parent node and marks them as orphans.
         * @param parent The parent node whose children will be detached.
         */
        void detach_all_children(const Node &parent);

        /**
         * @brief Gets the parent of a given node.
         * @param n The node for which to retrieve the parent.
         * @return The parent node.
         */
        Node get_parent(const Node &n) const {
            return parents[n];
        }

        /**
         * @brief Updates the attachment of a child node to a new parent.
         * @param child The child node to update.
         * @param new_parent The new parent node.
         */
        void update_attachment(const Node &child, const Node &new_parent);

        /**
         * @brief Updates the attachment of multiple child nodes to a new parent.
         * @param child_nodes A vector of child nodes to update.
         * @param new_parent The new parent node.
         */
        void update_attachments(const std::vector<Node> &child_nodes, const Node &new_parent);

        /**
         * @brief Replaces the attachment of multiple child nodes to a new parent.
         * Detaches each node from its current parent and attaches all nodes to the new parent.
         * @param child_nodes A vector of child nodes to reattach.
         * @param new_parent The new parent node.
         */
        void replace_attachment(const std::vector<Node> &child_nodes, const Node &new_parent);

        /**
         * @brief Sets multiple orphan nodes as children of a given parent node.
         * @pre All nodes in 'orphans' must be orphans.
         * @param parent The parent node.
         * @param orphans A vector of orphan nodes.
         */
        void set_orphans_as_children(const Node &parent, const std::vector<Node> &orphans);

        /**
         * @brief Gets the children of a given node.
         * @param n The node for which to retrieve children.
         * @return A constant reference to the vector of child nodes.
         */
        [[nodiscard]] const std::vector<Node> &get_children(const Node &n) const {
            return children[n];
        }

        /**
         * @brief Adds a child node to a given node.
         * @param n The node to which a child will be added.
         * @param child The child node to add.
         */
        void add_child(const Node &n, const Node &child) {
            children[n].push_back(child);
        }

        /**
         * @brief Removes a child from a given node.
         * @param parent The node from which to remove the child.
         * @param child The child node to remove.
         * @return True if the child was removed successfully, false otherwise.
         */
        bool remove_child(const Node &parent, const Node &child);

        /**
         * @brief Removes a child from a given node by index.
         * @param parent The node from which to remove the child.
         * @param child_idx The index of the child node to remove.
         * @return True if the child was removed successfully, false otherwise.
         */
        bool remove_child_idx(const Node &parent, const size_t &child_idx);

        /**
         * @brief Finds the index of a child node within the children of a parent node.
         * @param parent The parent node.
         * @param child The child node to find.
         * @return The index of the child node if found, otherwise BCG_INVALID_INDEX.
         */
        size_t find_child_idx(const Node &parent, const Node &child) const;

        /**
         * @brief Returns a range for depth-first search (DFS) traversal starting from the given root node.
         * @param root The root node to start the DFS traversal.
         * @return A DFSRange object representing the range for DFS traversal.
         */
        DFSRange<Tree> dfs(const Node &root) const {
            return DFSRange<Tree>(this, root);
        }

        /**
         * @brief Returns a range for breadth-first search (BFS) traversal starting from the given root node.
         * @param root The root node to start the BFS traversal.
         * @return A BFSRange object representing the range for BFS traversal.
         */
        BFSRange<Tree> bfs(const Node &root) const {
            return BFSRange<Tree>(this, root);
        }
    };
}

#endif //TREE_H
