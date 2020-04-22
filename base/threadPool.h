//
// Created by ralph on 4/21/20.
//

#ifndef LEARNMUDUO_THREADPOOL_H
#define LEARNMUDUO_THREADPOOL_H

#include <memory>
#include <vector>
#include <deque>
#include "alias.h"
#include "thread.h"

namespace ybase {

class ThreadPool {
NonCopyable(ThreadPool)
public:
    explicit ThreadPool(std::string poolName = std::string("ThreadPool"));
    ~ThreadPool();

    //call before start()
    void setMaxQueueSize(int maxSize) { m_maxQueueSize = maxSize; }
    void setThreadInitCallback(Task cb) { m_threadInitCb = std::move(cb); }

    void start(int numThreads);
    void stop();

    const std::string& getName() const { return m_poolName; }
    size_t getQueueSize() const;

    void run(Task f);

private:
    void runInThread();
    Task take();
    bool isFull() const;

private:
    std::string m_poolName;
    size_t m_maxQueueSize;
    bool m_running;

    mutable std::mutex m_mtx;
    std::condition_variable m_notEmpty;
    std::condition_variable m_notFull;

    std::vector<std::unique_ptr<Thread>> m_threads;
    Task m_threadInitCb;
    std::deque<Task> m_taskQueue;
};

} //namespace ybase

#endif //LEARNMUDUO_THREADPOOL_H
