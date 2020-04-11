//
// Created by ralph on 4/3/20.
//

#include <poll.h>
#include "eventLoop.h"
#include "../base/threadUtils.h"
#include "../base/logging.h"
#include "poller.h"

namespace ynet{
thread_local EventLoop* pLoopInThread = nullptr;

EventLoop* getEventLoopOfCurrentThread(){
    return pLoopInThread;
}

EventLoop::EventLoop()
: m_looping(false)
, m_threadId(ybase::ThreadUtils::getTid())
, m_quit(false){
    LOG_TRACE << "EventLoop " << this << "created in thread " << m_threadId;
    if(pLoopInThread){
        LOG_FATAL << "Another EventLoop " << pLoopInThread << "exists in this thread " << m_threadId;
    }else{
        pLoopInThread = this;
    }
}

EventLoop::~EventLoop() {
    assert(!m_looping);
    pLoopInThread = nullptr;
}

void EventLoop::loop() {
    assertInCurrentThread();
    assert(!m_looping);
    m_looping = true;
    m_quit = false;

    while(!m_quit){
        m_activeChannels.clear();
        m_poller->poll(1000, m_activeChannels);
        for(auto channel : m_activeChannels){
            channel->handleEvents();
        }
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    m_looping = false;
}

void EventLoop::assertInCurrentThread() {
    if(m_threadId != ybase::ThreadUtils::getTid()){
        LOG_FATAL << "EventLoop " << this << " is in thread " << m_threadId << " not in current thread" << ybase::ThreadUtils::getTid();
    }
}

void EventLoop::updateChannel(Channel *channel) {
    assert(channel->getOwnerLoop() == this);
    assertInCurrentThread();
    m_poller->updateChannel(channel);
}

} //namespace ynet