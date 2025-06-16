//
// Created by xshx on 2025/5/30.
//

#include <cstring>
#include "MiThreadPool.h"
#include "MiThreadPool.h"

void TaskEle::setFunc(function<void()> tcb){
    taskCallback = tcb;
}

void* ExecEle::start(void*arg){
    ExecEle *ee = (ExecEle*)arg;
    while(true){
        pthread_mutex_lock(&(ee->pool->mutex));
        while(ee->pool->task_queue.empty()){
            pthread_cond_wait(&ee->pool->cont, &ee->pool->mutex);
        }
        TaskEle *te = ee->pool->task_queue.front();
        ee->pool->task_queue.pop_front();
        pthread_mutex_unlock(&(ee->pool->mutex));
        te->taskCallback();
    }
    return NULL;
}

MiThreadPool::MiThreadPool() : thread_count(100) { // 默认线程池大小为100
    pthread_cond_init(&cont, NULL);
    pthread_mutex_init(&mutex, NULL);
}

void MiThreadPool::setThreadCount(int count) {
    thread_count = count;
}

void MiThreadPool::createPool(){
    int ret;
    for(int i = 0; i < thread_count; ++i){
        ExecEle *ee = new ExecEle;
        ee->pool = this;
        if((ret = pthread_create(&(ee->tid), NULL, ee->start, ee)) != 0){
            ALOGI("Failed to create thread %d: %s", i, strerror(ret));
            delete ee;
            // 清理已创建的线程
            for(int j = 0; j < i; ++j){
                pthread_join(exec_queue[j]->tid, NULL);
            }
            exec_queue.clear();
            pthread_cond_destroy(&cont);
            pthread_mutex_destroy(&mutex);
            return; // 安全退出
        }
        ALOGI("create thread %d", i);
        exec_queue.push_back(ee);
    }
    ALOGI("create pool finish...");
}

MiThreadPool::~MiThreadPool() {
    pthread_mutex_lock(&mutex);
    task_queue.clear();
    pthread_cond_broadcast(&cont);
    pthread_mutex_unlock(&mutex);
    for(auto& ee : exec_queue){
        pthread_join(ee->tid, NULL);
    }
    exec_queue.clear();
    pthread_cond_destroy(&cont);
    pthread_mutex_destroy(&mutex);
    ALOGI("Thread pool destroyed");
}