//
// Created by alex on 13.12.24.
//

#ifndef ENGINE25_JOBSYSTEM_H
#define ENGINE25_JOBSYSTEM_H

#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <atomic>
#include <stdexcept>

namespace Bcg {
    class JobSystem {
    public:
        // Constructs a JobSystem with the given number of worker threads.
        // If thread_count == 0, it uses std::thread::hardware_concurrency() as a fallback.
        explicit JobSystem(unsigned int thread_count = 0);

        ~JobSystem();

        // Enqueue a job that returns a result (using std::future)
        template<typename F, typename... Args>
        auto enqueue(F &&f, Args &&... args)
            -> std::future<typename std::invoke_result<F, Args...>::type> {
            using return_type = typename std::invoke_result<F, Args...>::type;

            auto task = std::make_shared<std::packaged_task<return_type()> >(
                [func = std::forward<F>(f), tup = std::make_tuple(std::forward<Args>(args)...)
                ]() mutable -> return_type {
                    return std::apply(func, tup);
                }
            );

            std::future<return_type> res = task->get_future(); {
                std::scoped_lock lock(mutex_);
                if (stop_flag_) {
                    throw std::runtime_error("Enqueue on stopped JobSystem");
                }
                tasks_.push([task]() { (*task)(); });
            }
            cv_.notify_one();
            return res;
        }

        // Enqueue a job that does not return a result
        void enqueue(std::function<void()> job);

        // Wait until all currently queued tasks are completed
        void wait();

    private:
        void WorkerThread();

        std::vector<std::thread> workers_;
        std::queue<std::function<void()> > tasks_;
        std::mutex mutex_;
        std::condition_variable cv_;
        std::condition_variable done_cv_;
        std::atomic<bool> stop_flag_;
        size_t active_tasks_ = 0;
    };
} // namespace Bcg
#endif //ENGINE25_JOBSYSTEM_H
