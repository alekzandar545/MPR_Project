#include "../include/thread_pool.hpp"


ThreadPool::ThreadPool(size_t threadCount) 
    : stop_(false)
{
    for (size_t i = 0; i < threadCount; ++i) {
        workers_.emplace_back([this] {
            workerLoop();
        });
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::enqueue(std::function<void()> job) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        jobs_.push(std::move(job));
    }
    cv_.notify_one();
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_ = true;
    }
    cv_.notify_all();

    for (auto &t : workers_) {
        if (t.joinable()) t.join();
    }
}

void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> job;

        {
            std::unique_lock<std::mutex> lock(mutex_);

            cv_.wait(lock, [this] {
                return stop_ || !jobs_.empty();
            });

            if (stop_ && jobs_.empty()) {
                return;
            }

            job = std::move(jobs_.front());
            jobs_.pop();
        }

        job();
    }
}

int ThreadPool::getThreadCount() const{
    return workers_.size();
}