//
// Created by alex on 2/3/25.
//

#include "Tree.h"

namespace Bcg {
    Tree::Tree() {
        n_deleted = node_property<bool>("n:deleted", false);
        parents = node_property<Node>("n:parent", Node());
        children = node_property<std::vector<Node> >("n:children", std::vector<Node>());
    }

    Tree &Tree::operator=(const Tree &rhs) {
        if (this != &rhs) {
            nodes = rhs.nodes;
            n_deleted = rhs.n_deleted;
            parents = rhs.parents;
            children = rhs.children;
        }
        return *this;
    }

    Tree &Tree::assign(const Tree &rhs) {
        if (this != &rhs) {
            // clear properties
            clear();

            // resize (needed by property containers)
            nodes.resize(rhs.nodes.size());
            n_deleted.vector() = rhs.n_deleted.vector();
            parents.vector() = rhs.parents.vector();
            children.vector() = rhs.children.vector();

            // how many elements are deleted?
            nodes.num_deleted = rhs.nodes.num_deleted;
        }

        return *this;
    }

    void Tree::clear() {
        nodes.clear();
        n_deleted = node_property<bool>("n:deleted", false);
        parents = node_property<Node>("n:parent", Node());
        children = node_property<std::vector<Node> >("n:children", std::vector<Node>());
    }

    void Tree::free_memory() {
        nodes.free_memory();
    }

    void Tree::reserve(size_t nnodes) {
        nodes.reserve(nnodes);
    }

    void Tree::garbage_collection() {
        // If there is no garbage, nothing to do.
        if (!has_garbage()) {
            return;
        }

        // Step 1. Build a mapping from old indices to new indices.
        // Assume nodes.size() returns the total number of nodes.
        auto nN = nodes.size();
        auto nmap = node_property<Node>("n:garbage_collection");

        for (size_t i = 0; i < nN; ++i) {
            nmap[Node(i)] = Node(i);
        }

        // remove deleted vertices
        if (nN > 0) {
            size_t i0 = 0;
            size_t i1 = nN - 1;

            while (true) {
                // find first deleted and last un-deleted
                while (!n_deleted[Node(i0)] && i0 < i1) {
                    ++i0;
                }
                while (n_deleted[Node(i1)] && i0 < i1) {
                    --i1;
                }
                if (i0 >= i1) {
                    break;
                }

                // swap
                nodes.swap(i0, i1);
            }

            // remember new size
            nN = n_deleted[Node(i0)] ? i0 : i0 + 1;
        }

        //update attachments
        for (size_t i = 0; i < nN; ++i) {
            auto n = Node(i);
            if (!is_orphan(n)) {
                parents[n] = nmap[parents[n]];
            }
            for (auto &child: children[n]) {
                child = nmap[child];
            }
        }

        remove_node_property(nmap);

        // finally resize arrays
        nodes.resize(nN);
        free_memory();

        nodes.num_deleted = 0;
    }

    void Tree::mark_deleted(const Node &n) {
        if (n_deleted[n])
            return;

        n_deleted[n] = true;
        ++nodes.num_deleted;
    }

    void Tree::delete_node(const Node &n) {
        mark_deleted(n);

        // remove children
        for (const auto &child: children[n]) {
            mark_deleted(child);
        }

        // remove parent
        parents[n] = Node();
    }

    void Tree::attach_to_parent(const Node &n, const Node &parent) {
        if (!is_orphan(n)) {
            detach_from_parent(n);
        }
        parents[n] = parent;
        children[parent].push_back(n);
    }

    void Tree::attach_orphans_to_parent(const std::vector<Node> &orphans, const Node &new_parent) {
        for (const auto &n: orphans) {
            assert(is_orphan(n));
            parents[n] = new_parent;
        }
        children[new_parent].insert(children[new_parent].end(), orphans.begin(), orphans.end());
    }

    void Tree::detach_from_parent(const Node &child) {
        if (is_orphan(child)) {
            return;
        }
        if (remove_child(parents[child], child)) {
            parents[child] = Node();
        }
    }

    void Tree::detach_from_parents(const std::vector<Node> &nodes_to_detach) {
        for (const Node &n: nodes_to_detach) {
            detach_from_parent(n);
        }
    }

    void Tree::detach_all_children(const Node &parent) {
        std::vector<Node> &child_nodes = children[parent];
        for (const Node &child: child_nodes) {
            parents[child] = Node();
        }
        child_nodes.clear();
    }

    void Tree::update_attachment(const Node &child, const Node &new_parent) {
        if (!is_orphan(child)) {
            detach_from_parent(child);
        }
        assert(is_orphan(child));
        attach_orphan_to_parent(child, new_parent);
    }

    void Tree::update_attachments(const std::vector<Node> &child_nodes, const Node &new_parent) {
        for (const Node &child: child_nodes) {
            if (!is_orphan(child)) {
                detach_from_parent(child);
            }
        }
        attach_orphans_to_parent(child_nodes, new_parent);
    }

    void Tree::replace_attachment(const std::vector<Node> &child_nodes, const Node &new_parent) {
        for (const auto &child: child_nodes) {
            if (!is_orphan(child)) {
                detach_from_parent(child);
            }
            parents[child] = new_parent;
        }
        children[new_parent] = child_nodes;
    }

    void Tree::set_orphans_as_children(const Node &parent, const std::vector<Node> &orphans) {
        for (const auto &child: orphans) {
            assert(is_orphan(child));
            parents[child] = parent;
        }
        children[parent] = orphans;
    }

    bool Tree::remove_child(const Node &parent, const Node &child) {
        return remove_child_idx(parent, find_child_idx(parent, child));
    }

    bool Tree::remove_child_idx(const Node &parent, const size_t &child_idx) {
        if (child_idx >= children[parent].size()) {
            return false;
        }
        std::swap(children[parent][child_idx], children[parent].back());
        children[parent].pop_back();
        return true;
    }

    size_t Tree::find_child_idx(const Node &parent, const Node &child) const {
        const std::vector<Node> &child_nodes = get_children(parent);
        for (size_t i = 0; i < child_nodes.size(); ++i) {
            if (child_nodes[i] == child) {
                return i;
            }
        }
        return BCG_INVALID_IDX;
    }
}
