//
// Created by ralph on 4/3/20.
//

#include "Thread.h"
#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

namespace ybase {

std::atomic<int> Thread::numCreated;

Thread::Thread(Thread::ThreadFunc func, std::string name)
        : m_started(false), m_joined(false), m_pthreadId(0), m_tid(0), m_func(std::move(func)),
          m_name(std::move(name)) {
    ++numCreated;
    setDefaultName();
}

Thread::~Thread() {
    if (m_started && !m_joined)
        pthread_detach(m_pthreadId);
}

void Thread::setDefaultName() {
    int num = numCreated.load();
    if (m_name.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread:%d", num);
        m_name = buf;
    }
}

void Thread::start() {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&m_pthreadId, &attr, reinterpret_cast<void *(*)(void *)>(&m_func), 0);
}

    int Thread::join() {
        return 0;
    }


} //namespace ybase
