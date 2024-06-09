//
// Created by Aryan Singh on 6/7/24.
//

#ifndef GRAPHICA_THREAD_POOL_H
#define GRAPHICA_THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <iostream>


class ThreadPool {
public:
    ThreadPool(size_t num_threads)
            : stop(false), active_tasks(0) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back(&ThreadPool::worker_thread, this);
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    template<typename F>
    void enqueue(F&& task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::forward<F>(task));
        }
        condition.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        finished.wait(lock, [this]() { return tasks.empty() && (active_tasks == 0); });
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::condition_variable finished;
    std::atomic<bool> stop;
    std::atomic<int> active_tasks;

    void worker_thread() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                condition.wait(lock, [this]() { return stop || !tasks.empty(); });
                if (stop && tasks.empty()) {
                    return;
                }
                task = std::move(tasks.front());
                tasks.pop();
                ++active_tasks;
            }
            task();
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                --active_tasks;
            }
            finished.notify_one();
        }
    }
};



#endif //GRAPHICA_THREAD_POOL_H
