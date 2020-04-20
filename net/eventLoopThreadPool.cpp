//
// Created by ralph on 4/20/20.
//

#include "eventLoopThreadPool.h"
#include "eventLoop.h"
#include "eventLoopThread.h"

namespace ynet{


EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, std::string poolName)
: m_baseLoop(baseLoop)
, m_poolName(std::move(poolName))
, m_started(false)
, m_numThreads(0)
, m_next(0){

}

EventLoopThreadPool::~EventLoopThreadPool() = default;

EventLoop *EventLoopThreadPool::getNextLoop() {
    m_baseLoop->assertInCurrentThread();
    assert(m_started);
    EventLoop* loop = m_baseLoop;

    if(!m_loops.empty()){
        //round-robin
        loop = m_loops[m_next++];
        if(m_next >= m_loops.size())
            m_next = 0;
    }
    return loop;
}

EventLoop *EventLoopThreadPool::getLoopForHash(size_t hashCode) {
    m_baseLoop->assertInCurrentThread();
    EventLoop* loop = m_baseLoop;

    if(!m_loops.empty())
        loop = m_loops[hashCode % m_loops.size()];
    return loop;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops() {
    m_baseLoop->assertInCurrentThread();
    assert(m_started);
    if(m_loops.empty())
        return std::vector<EventLoop*>(1, m_baseLoop);
    else
        return m_loops;
}

void EventLoopThreadPool::start(const LoopThreadInitCallback &cb) {
    assert(!m_started);
    m_baseLoop->assertInCurrentThread();

    m_started = true;
    for(int i = 0; i < m_numThreads; ++i){
        char buf[m_poolName.size() + 32];
        snprintf(buf, sizeof(buf), "%s%d", m_poolName.c_str(), i);
        EventLoopThread* t = new EventLoopThread(cb, buf);
        m_threads.push_back(std::unique_ptr<EventLoopThread>(t));
        m_loops.push_back(t->startLoop());
    }

    if(m_numThreads == 0 && cb)
        cb(m_baseLoop);
}

} //namespace ynet