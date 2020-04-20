//
// Created by ralph on 4/20/20.
//

#ifndef LINUXSERVER_EVENTLOOPTHREADPOOL_H
#define LINUXSERVER_EVENTLOOPTHREADPOOL_H

#include <memory>
#include <vector>
#include "alias.h"

namespace ynet{
class EventLoop;
class EventLoopThread;

class EventLoopThreadPool {
NonCopyable(EventLoopThreadPool)

public:
    EventLoopThreadPool(EventLoop* baseLoop, std::string poolName);
    ~EventLoopThreadPool();

    EventLoop* getNextLoop(); //round-robin
    EventLoop* getLoopForHash(size_t hashCode);

    std::vector<EventLoop*> getAllLoops();

    void setThreadNum(int numThreads) { m_numThreads = numThreads; }
    void start(const LoopThreadInitCallback &cb = LoopThreadInitCallback());
    bool getStarted() const { return m_started; }
    const std::string& getPoolName() const { return m_poolName; }

private:
    EventLoop* m_baseLoop;
    std::string m_poolName;
    bool m_started;
    int m_numThreads; //not including baseLoop
    int m_next;

    std::vector<std::unique_ptr<EventLoopThread>> m_threads;
    std::vector<EventLoop*> m_loops;
};

} //namespace ynet

#endif //LINUXSERVER_EVENTLOOPTHREADPOOL_H
