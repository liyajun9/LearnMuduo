//
// Created by ralph on 4/21/20.
//

#include <cassert>
#include "threadPool.h"
#include "exception.h"

namespace ybase{


ThreadPool::ThreadPool(std::string poolName)
: m_poolName(std::move(poolName))
, m_maxQueueSize(0)
, m_running(false){

}

ThreadPool::~ThreadPool() {
    if(m_running)
        stop();
}

void ThreadPool::start(int numThreads) {
    assert(m_threads.empty());
    m_running = true;
    m_threads.reserve(numThreads);
    for(int i = 0; i < numThreads; ++i){
        char id[32];
        snprintf(id, sizeof(id), "%d", i+1);
        m_threads.emplace_back(new Thread([this](){
            this->runInThread();
        }, m_poolName + id));

        m_threads[i]->start();
    }

    if(numThreads == 0 && m_threadInitCb)
        m_threadInitCb();
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_running = false;
        m_notEmpty.notify_all();
    }
    for(auto& thread : m_threads)
        thread->join();
}

size_t ThreadPool::getQueueSize() const {
    std::lock_guard<std::mutex> lock(m_mtx);
    return m_taskQueue.size();
}

void ThreadPool::run(Task task) {
    if(m_threads.empty()){
        task();
    }else{
        std::unique_lock<std::mutex>(m_mtx);
        while(isFull()){
            m_notFull.wait(m_mtx);
        }
        assert(!isFull());

        m_taskQueue.push_back(std::move(task));
        m_notEmpty.notify_one();
    }
}

void ThreadPool::runInThread() {
    try{
        if(m_threadInitCb)
            m_threadInitCb();

        while(m_running){
            Task task(take());
            if(task){
                task();
            }
        }
    }catch(const Exception& ex){
        fprintf(stderr, "exception caught in ThreadPool %s\n", m_poolName.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
        abort();
    }catch(const std::exception& ex){
        fprintf(stderr, "exception caught in ThreadPool %s\n", m_poolName.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }catch(...){
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", m_poolName.c_str());
        throw; // rethrow
    }
}

Task ThreadPool::take() {
    std::unique_lock<std::mutex> lock(m_mtx);
    while(m_taskQueue.empty() && m_running){ //wait for not empty
        m_notEmpty.wait(lock);
    }
    Task task;
    if(!m_taskQueue.empty()){
        task = m_taskQueue.front();
        m_taskQueue.pop_front();
        if(m_maxQueueSize > 0)
            m_notFull.notify_one();
    }

    return task;
}

bool ThreadPool::isFull() const {
    return false;
}

} //namespace ybase
