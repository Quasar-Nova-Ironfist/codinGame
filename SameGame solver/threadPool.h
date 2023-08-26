#pragma once
#include "main.h"
#include <thread>
#include <mutex>
#include <condition_variable>
struct threadPool{//thread pool specialized for this one specific fucking task. I never want to see #include<functional> again
    std::condition_variable task_available_cv = {}, tasks_done_cv = {};
    std::vector<populateMapWorkerArgs> tasks;
    int tasks_running = 0;
    mutable std::mutex tasks_mutex = {};
    int thread_count = 0;
    std::unique_ptr<std::thread[]> threads = nullptr;
    bool waiting = false, workers_running = true;
    void worker() {
        while (true)
        {
            std::unique_lock tasks_lock(tasks_mutex);
            task_available_cv.wait(tasks_lock, [this] { return !tasks.empty() || !workers_running; });
            if (!workers_running)
                break;
            populateMapWorkerArgs task = std::move(tasks.back());
            tasks.pop_back();
            ++tasks_running;
            tasks_lock.unlock();
            populateMapWorker(task);
            tasks_lock.lock();
            --tasks_running;
            if (waiting && !tasks_running && tasks.empty())
                tasks_done_cv.notify_all();
        }
    }
    void wait_for_tasks() {
        std::unique_lock tasks_lock(tasks_mutex);
        waiting = true;
        tasks_done_cv.wait(tasks_lock, [this] { return !tasks_running && tasks.empty(); });
        waiting = false;
    }
    threadPool(int thread_count_) : thread_count(thread_count_), threads(std::make_unique<std::thread[]>(thread_count_)) {
        for (unsigned int i = 0; i < thread_count; ++i)
            threads[i] = std::thread(&threadPool::worker, this);
    }
    ~threadPool() {
        std::unique_lock tasks_lock(tasks_mutex);
        waiting = true;
        tasks_done_cv.wait(tasks_lock, [this] { return !tasks_running && tasks.empty(); });
        workers_running = false;
        tasks_lock.unlock();
        task_available_cv.notify_all();
        for (unsigned int i = 0; i < thread_count; ++i)
            threads[i].join();
    }
};