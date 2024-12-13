//
// Created by alex on 13.12.24.
//

#include "JobSystem.h"

namespace Bcg {
    JobSystem::JobSystem(unsigned int thread_count) {
        if (thread_count == 0) {
            thread_count = std::thread::hardware_concurrency();
            if (thread_count == 0) {
                thread_count = 1;
            }
        }

        stop_flag_ = false;
        for (unsigned int i = 0; i < thread_count; ++i) {
            workers_.emplace_back([this]() {
                WorkerThread();
            });
        }
    }

    JobSystem::~JobSystem() {
        {
            std::scoped_lock lock(mutex_);
            stop_flag_ = true;
        }
        cv_.notify_all();
        for (auto &worker: workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    void JobSystem::Enqueue(std::function<void()> job) {
        {
            std::scoped_lock lock(mutex_);
            if (stop_flag_) {
                throw std::runtime_error("Enqueue on stopped JobSystem");
            }
            tasks_.push(job);
        }
        cv_.notify_one();
    }

    void JobSystem::Wait() {
        std::unique_lock lock(mutex_);
        done_cv_.wait(lock, [this]() {
            return tasks_.empty() && active_tasks_ == 0;
        });
    }

    void JobSystem::WorkerThread() {
        for (;;) {
            std::function<void()> task;
            {
                std::unique_lock lock(mutex_);
                cv_.wait(lock, [this]() {
                    return stop_flag_ || !tasks_.empty();
                });

                if (stop_flag_ && tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop();
                ++active_tasks_;
            }

            // Execute the task outside the lock
            task();

            {
                std::scoped_lock lock(mutex_);
                --active_tasks_;
                if (tasks_.empty() && active_tasks_ == 0) {
                    done_cv_.notify_all();
                }
            }
        }
    }
}