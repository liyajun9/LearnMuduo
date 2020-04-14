//
// Created by ralph on 4/14/20.
//

#include "eventLoopThread.h"

#include <utility>

namespace ynet{


EventLoopThread::EventLoopThread(LoopThreadInitCallback cb, const std::string &name)
: m_initCallback(std::move(cb))
, m_exiting(false)
, m_thread([this](){this->ThreadFunc();})
, m_loop(nullptr){

}

EventLoop *EventLoopThread::startLoop() {
    assert(!m_thread.started());
    m_thread.start();
    m_loop = nullptr;

    //must wait for loop created, so that the thread could be used by client
    {
        std::unique_lock<std::mutex> lock(m_mtx);
        while (!m_loop) {
            m_cond.wait(lock);
        }
    }
    return m_loop;
}

EventLoopThread::~EventLoopThread() {
    m_exiting = true;
    if(m_loop){
        m_loop->quit_mt();
        m_thread.join();
    }
}

void EventLoopThread::ThreadFunc() {
    EventLoop loop;
    if(m_initCallback)
        m_initCallback(&loop);

    {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_loop = &loop;
        m_cond.notify_all();
    }
    loop.loop();

    std::lock_guard<std::mutex> lock(m_mtx);
    m_loop = nullptr;
}


} //namespace ynet