//
// Created by ralph on 4/3/20.
//

#include "eventLoop.h"
#include "../base/threadUtils.h"

namespace ynet{
thread_local EventLoop* pLoopInThread = 0;

EventLoop::EventLoop()
:m_looping(false)
,m_threadId(ybase::ThreadUtils::getTid()){
    pLoopInThread = this;
}

EventLoop::~EventLoop() {
    assert(!m_looping);
    pLoopInThread = nullptr;
}

} //namespace ynet