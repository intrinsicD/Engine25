//
// Created by alex on 13.12.24.
//

#ifndef ENGINE25_TASKGRAPH_H
#define ENGINE25_TASKGRAPH_H

#include "Dag.h"
#include "Command.h"

namespace Bcg {

    class TaskGraph : public Dag<Task> {
    public:
        ~TaskGraph() override = default;

        // Fully sequential: run all tasks in topological order on the calling thread
        void ExecuteSequential();

        // Parallel but naive:
        // Enqueue all tasks from topological sort into the job system at once and then wait.
        // This does not enforce that tasks start only after their dependencies finish, but
        // because there's no cycle, tasks won't break ordering logically.
        // Any data dependencies must be handled inside tasks themselves if needed.
        void ExecuteParallelNaive();

        // Full parallel (level-by-level):
        // We compute in-degrees for each node. We start by enqueuing tasks with no dependencies.
        // Once they complete, we decrement the in-degree of their dependents.
        // When a dependent has in-degree zero, we enqueue it in the next "wave."
        // We proceed level by level until all tasks are done.
        void ExecuteFullParallelUsingJobSystem();

    private:
        // We assume Dag<Task> or Dag<NodeData> has a protected or public method to get adjacency list.
        // If not, we can add one to Dag. For example:

        const std::vector<std::vector<size_t>> &GetAdjacencyList() const;
    };

}

#endif //ENGINE25_TASKGRAPH_H
