//
// Created by ralph on 4/14/20.
//

#ifndef LINUXSERVER_EVENTLOOPTHREAD_H
#define LINUXSERVER_EVENTLOOPTHREAD_H

#include "eventLoop.h"
#include "../base/thread.h"
#include "alias.h"

namespace ynet {

class EventLoopThread {
    NonCopyable(EventLoopThread)

public:
    explicit EventLoopThread(LoopThreadInitCallback  cb = LoopThreadInitCallback(),
                    const std::string& name = std::string());
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void ThreadFunc();

private:
    EventLoop* m_loop;

    ybase::Thread m_thread;
    bool m_exiting;
    LoopThreadInitCallback m_initCallback;
    std::mutex m_mtx;
    std::condition_variable m_cond;
};

} //namespace ynet

#endif //LINUXSERVER_EVENTLOOPTHREAD_H
