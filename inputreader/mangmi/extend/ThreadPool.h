//
// Created by xshx on 2025/6/12.
//

#ifndef UNTITLED_THREADPOOL_H
#define UNTITLED_THREADPOOL_H

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

//class ThreadPool {
//public:
//    // 构造函数：创建指定数量的工作线程
//    ThreadPool(size_t threads);
//
//    // 析构函数：自动关闭线程池
//    ~ThreadPool();
//
//    // 提交任务到线程池
//    template<class F, class... Args>
//    auto enqueue(F&& f, Args&&... args)
//    -> std::future<typename std::result_of<F(Args...)>::type>;
//
//    // 等待所有任务完成并关闭线程池
//    void shutdown();
//
//    // 立即关闭线程池，丢弃未执行的任务
//    void shutdownNow();
//
//    // 获取线程池状态
//    bool isRunning() const;
//    size_t getQueueSize() const;
//    size_t getThreadCount() const;
//
////private:
//    // 工作线程组
//    std::vector<std::thread> workers;
//
//    // 任务队列
//    std::queue<std::function<void()>> tasks;
//
//    // 同步原语
//    std::mutex queue_mutex;
//    std::condition_variable condition;
//
//    // 线程池状态
//    std::atomic<bool> stop{false};
//    std::atomic<bool> discard_pending{false};
//};

#endif //UNTITLED_THREADPOOL_H
