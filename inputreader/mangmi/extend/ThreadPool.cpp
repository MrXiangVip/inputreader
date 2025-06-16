//
// Created by xshx on 2025/6/12.
//

#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <atomic>
#include "ThreadPool.h"
// 构造函数实现
ThreadPool::ThreadPool(size_t threads) : stop(false), discard_pending(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    // 加锁等待任务或退出信号
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });

                    // 如果线程池停止且队列为空或需要丢弃待执行任务，退出线程
                    if (this->stop && (this->tasks.empty() || this->discard_pending))
                        return;

                    // 获取任务
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }

                // 执行任务（解锁状态下）
                try {
                    task();
                } catch (const std::exception& e) {
                    std::cerr << "Task exception: " << e.what() << std::endl;
                }
            }
        });
    }
}

// 任务提交函数实现
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    // 如果线程池已停止，拒绝新任务
    if (stop.load()) {
        throw std::runtime_error("enqueue on stopped ThreadPool");
    }

    // 打包任务
    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace([task]() { (*task)(); });
    }

    // 通知一个等待的线程
    condition.notify_one();
    return res;
}

// 优雅关闭线程池
void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop.exchange(true)) return;  // 已关闭，直接返回
        discard_pending.store(false);  // 不丢弃待执行任务
    }

    // 通知所有线程
    condition.notify_all();

    // 等待所有线程完成
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

// 立即关闭线程池
void ThreadPool::shutdownNow() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop.exchange(true)) return;  // 已关闭，直接返回
        discard_pending.store(true);  // 丢弃待执行任务

        // 清空任务队列
        std::queue<std::function<void()>> empty;
        std::swap(tasks, empty);
    }

    // 通知所有线程
    condition.notify_all();

    // 等待所有线程完成
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

// 获取线程池状态
bool ThreadPool::isRunning() const {
    return !stop.load();
}

size_t ThreadPool::getQueueSize() const {
//    std::unique_lock<std::mutex> lock(queue_mutex);
    return tasks.size();
}

size_t ThreadPool::getThreadCount() const {
    return workers.size();
}

// 析构函数
ThreadPool::~ThreadPool() {
    if (!stop.load()) {
        shutdown();
    }
}
