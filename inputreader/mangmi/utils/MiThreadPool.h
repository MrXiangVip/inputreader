//
// Created by xshx on 2025/5/30.
//

#ifndef UNTITLED_MITHREADPOOL_H
#define UNTITLED_MITHREADPOOL_H


#include "MiThreadPool.h"
#include <deque>
#include <string>
#include <iostream>
#include <pthread.h>
#include <functional>
#include <tuple>
#include <utility>
#include "../Macro.h"
using namespace std;

class MiThreadPool;

//-任务队列元素
class TaskEle{
public:
    function<void()> taskCallback;
    void setFunc(function<void()> tcb);
};

//-执行队列元素
class ExecEle{
public:
    pthread_t tid;
    MiThreadPool* pool;
    static void* start(void* arg);
};

//-线程池
class MiThreadPool{
public:
    deque<TaskEle*> task_queue;
    deque<ExecEle*> exec_queue;
    pthread_cond_t cont;
    pthread_mutex_t mutex;
    int thread_count; // 线程池大小
    MiThreadPool();
    void setThreadCount(int count);
    void createPool();
    template<typename Func, typename... Args>
    void push_task(Func&& func, Args&&... args) {
        TaskEle *te = new TaskEle;
        te->setFunc(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        pthread_mutex_lock(&mutex);
        task_queue.push_back(te);
        pthread_cond_signal(&cont);
        pthread_mutex_unlock(&mutex);
        ALOGI("Task pushed to queue");
    }
    ~MiThreadPool();
};


#endif //UNTITLED_MITHREADPOOL_H
