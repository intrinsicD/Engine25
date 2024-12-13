//
// Created by alex on 13.12.24.
//

#ifndef ENGINE25_DAG_H
#define ENGINE25_DAG_H

#include <vector>
#include <algorithm>
#include <stdexcept>

namespace Bcg {
    // A generic Directed Acyclic Graph (DAG) class.
    // NodeData: The type of data stored in each node (e.g., a task or a render pass description).

    template <typename NodeData>
    class Dag {
    public:
        struct Node {
            NodeData data;
        };

        enum class VisitState { NotVisited, Visiting, Visited };

        virtual ~Dag() = default;

        // Add a new node to the DAG and return its ID (index).
        size_t AddNode(const NodeData &data) {
            nodes_.push_back({data});
            adjacency_list_.push_back({});
            return nodes_.size() - 1;
        }

        // Add a directed edge from 'from' to 'to', indicating 'to' depends on 'from'.
        void AddEdge(size_t from, size_t to) {
            if (from >= nodes_.size() || to >= nodes_.size()) {
                throw std::out_of_range("Invalid node index for AddEdge");
            }
            adjacency_list_[from].push_back(to);
        }

        // Perform a topological sort of the DAG.
        // Returns a vector of node IDs in a valid topological order.
        // Throws std::runtime_error if a cycle is detected.
        std::vector<size_t> TopologicalSort() const {
            std::vector<size_t> result;
            result.reserve(nodes_.size());

            std::vector<VisitState> state(nodes_.size(), VisitState::NotVisited);

            for (size_t i = 0; i < nodes_.size(); ++i) {
                if (state[i] == VisitState::NotVisited) {
                    DfsVisit(i, state, result);
                }
            }

            std::reverse(result.begin(), result.end());
            return result;
        }

        Node &GetNode(size_t id) {
            if (id >= nodes_.size()) {
                throw std::out_of_range("Invalid node index for GetNode");
            }
            return nodes_[id];
        }

        const Node &GetNode(size_t id) const {
            if (id >= nodes_.size()) {
                throw std::out_of_range("Invalid node index for GetNode");
            }
            return nodes_[id];
        }

        // Clear the DAG so it can be rebuilt (useful when constructing each frame).
        void Clear() {
            nodes_.clear();
            adjacency_list_.clear();
        }

        size_t GetNodeCount() const {
            return nodes_.size();
        }

    protected:
        void DfsVisit(size_t node, std::vector<enum VisitState> &state, std::vector<size_t> &result) const {
            if (state[node] == VisitState::Visiting) {
                // Found a node already in the visiting stack -> cycle detected
                throw std::runtime_error("Cycle detected in DAG");
            }
            if (state[node] == VisitState::NotVisited) {
                state[node] = VisitState::Visiting;
                for (auto &n : adjacency_list_[node]) {
                    DfsVisit(n, state, result);
                }
                state[node] = VisitState::Visited;
                result.push_back(node);
            }
        }

        std::vector<Node> nodes_;
        std::vector<std::vector<size_t>> adjacency_list_;
    };

} // namespace Bcg

#endif //ENGINE25_DAG_H
