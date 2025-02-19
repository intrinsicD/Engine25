//
// Created by alex on 13.12.24.
//

#include <queue>
#include <future>
#include "TaskGraph.h"
#include "JobSystem.h"
#include "Engine.h"

namespace Bcg {
    // Fully sequential: run all tasks in topological order on the calling thread
    void TaskGraph::ExecuteSequential() {
        auto sorted = TopologicalSort();
        for (auto id: sorted) {
            auto &task = GetNode(id).data;
            task.Execute();
        }
    }

    // Parallel but naive:
    // Enqueue all tasks from topological sort into the job system at once and then wait.
    // This does not enforce that tasks start only after their dependencies finish, but
    // because there's no cycle, tasks won't break ordering logically.
    // Any data dependencies must be handled inside tasks themselves if needed.
    void TaskGraph::ExecuteParallelNaive() {
        auto &jobSystem = Engine::get_context().get<Bcg::JobSystem>();
        auto sorted = TopologicalSort();

        std::vector<std::future<void>> futures;
        futures.reserve(sorted.size());

        for (auto id: sorted) {
            auto &task = GetNode(id).data;
            futures.push_back(jobSystem.Enqueue([&task]() {
                task.Execute();
            }));
        }

        // Wait for all tasks to complete
        for (auto &f: futures) {
            f.get();
        }
    }

    // Full parallel (level-by-level):
    // We compute in-degrees for each node. We start by enqueuing tasks with no dependencies.
    // Once they complete, we decrement the in-degree of their dependents.
    // When a dependent has in-degree zero, we enqueue it in the next "wave."
    // We proceed level by level until all tasks are done.
    void TaskGraph::ExecuteFullParallelUsingJobSystem() {
        auto &jobSystem = Engine::get_context().get<Bcg::JobSystem>();

        // Compute in-degrees
        std::vector<size_t> in_degree(GetNodeCount(), 0);
        // Build adjacency info. The Dag base class holds adjacency_list_ internally;
        // we assume it provides a method GetAdjacencyList() or we modify Dag to allow it.
        // If not directly accessible, consider adding a protected getter in Dag.
        const auto &adj_list = GetAdjacencyList();

        for (size_t u = 0; u < GetNodeCount(); ++u) {
            for (auto v: adj_list[u]) {
                in_degree[v]++;
            }
        }

        // Initialize a queue with all zero in-degree tasks
        std::queue<size_t> current_level;
        for (size_t i = 0; i < GetNodeCount(); ++i) {
            if (in_degree[i] == 0) {
                current_level.push(i);
            }
        }

        // Process level-by-level
        while (!current_level.empty()) {
            std::vector<std::future<void>> futures;
            std::vector<size_t> this_level_nodes;

            // Dequeue all tasks from current_level into a temporary vector
            // so we can run them all in parallel.
            while (!current_level.empty()) {
                this_level_nodes.push_back(current_level.front());
                current_level.pop();
            }

            // Enqueue all tasks in this level
            for (auto node_id: this_level_nodes) {
                auto &task = GetNode(node_id).data;
                futures.push_back(jobSystem.Enqueue([&task]() {
                    task.Execute();
                }));
            }

            // Wait for all tasks in this level to finish
            for (auto &f: futures) {
                f.get();
            }

            // Now that these tasks are done, reduce the in-degree of their children
            // and enqueue any that are now ready.
            for (auto node_id: this_level_nodes) {
                for (auto dep: adj_list[node_id]) {
                    in_degree[dep]--;
                    if (in_degree[dep] == 0) {
                        current_level.push(dep);
                    }
                }
            }
        }

        // By the time we exit this loop, all tasks have been completed.
    }

    const std::vector<std::vector<size_t>> &TaskGraph::GetAdjacencyList() const {
        // This method must be added to Dag if adjacency_list_ is private in Dag.
        // For the sake of this example, assume Dag has a protected getter or we made it public.
        return adjacency_list_;
    }
}