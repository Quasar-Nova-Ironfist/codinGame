#pragma once
#include "main.h"
#include <thread>
#include <mutex>
#include <condition_variable>
class threadPool{//thread pool specialized for this one specific fucking task. I never want to see #include<functional> again
    std::condition_variable task_available_cv = {}, tasks_done_cv = {};
    int tasks_running = 0;
    mutable std::mutex tasks_mutex = {};
    int thread_count = 0;
    std::unique_ptr<std::thread[]> threads = nullptr;
    bool waiting = false, workers_running = true;

    std::vector<board> qBoard;
    std::vector<node*> qNodePtr;
    std::vector<std::vector<std::pair<int, int>>> qMove;
    std::vector<int> qI;

    friend void populateMap(board& b, node* n);
    void worker() {
        while (true)
        {
            std::unique_lock tasks_lock(tasks_mutex);
            task_available_cv.wait(tasks_lock, [this] { return !qBoard.empty() || !workers_running; });
            if (!workers_running)
                break;
            board b = std::move(qBoard.back());
            qBoard.pop_back();
            node* n = qNodePtr.back();
            qNodePtr.pop_back();
            std::vector<std::pair<int, int>> m = std::move(qMove.back());
            qMove.pop_back();
            int i = qI.back();
            qI.pop_back();            
            ++tasks_running;
            tasks_lock.unlock();
            populateMapWorker(std::move(b), n, std::move(m), i);
            tasks_lock.lock();
            --tasks_running;
            if (waiting && !tasks_running && qBoard.empty())
                tasks_done_cv.notify_all();
        }
    }
public:
    void wait_for_tasks() {
        std::unique_lock tasks_lock(tasks_mutex);
        waiting = true;
        tasks_done_cv.wait(tasks_lock, [this] { return !tasks_running && qBoard.empty(); });
        waiting = false;
    }
    threadPool(int thread_count_) : thread_count(thread_count_), threads(std::make_unique<std::thread[]>(thread_count_)) {
        for (unsigned int i = 0; i < thread_count; ++i)
            threads[i] = std::thread(&threadPool::worker, this);
    }
    ~threadPool() {
        std::unique_lock tasks_lock(tasks_mutex);
        waiting = true;
        tasks_done_cv.wait(tasks_lock, [this] { return !tasks_running && qBoard.empty(); });
        workers_running = false;
        tasks_lock.unlock();
        task_available_cv.notify_all();
        for (unsigned int i = 0; i < thread_count; ++i)
            threads[i].join();
    }
};