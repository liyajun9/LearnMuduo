//
// Created by ralph on 4/3/20.
//

#include "thread.h"
#include "threadUtils.h"
#include "exception.h"
#include <cassert>
#include <sys/prctl.h>

namespace ybase {

std::atomic<int> Thread::numCreated;

Thread::Thread(ThreadFunc func, std::string name)
: m_started(false)
, m_joined(false)
, m_pthreadId(0)
, m_tid(0)
, m_func(std::move(func))
,m_latch(1)
,m_name(std::move(name)) {
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
    assert(!m_started);
    m_started = true;
    if(pthread_create(&m_pthreadId, nullptr, reinterpret_cast<void *(*)(void *)>(&run), this)){
        m_started = false;
        std::cout << "failed in pthread_create" << std::endl;
    }else{
        m_latch.wait(); //wait for getTid cached
        assert(m_tid > 0);
    }
}

int Thread::join() {
    assert(m_started);
    assert(!m_joined);
    m_joined = true;
    return pthread_join(m_pthreadId, nullptr);
}

void *Thread::run(void *arg) {
    assert(arg);
    Thread* pThrd = reinterpret_cast<Thread*>(arg);
    pThrd->m_tid = ThreadUtils::getTid();
    pThrd->m_latch.countDown();
    tls_threadName = pThrd->m_name.c_str();
    ::prctl(PR_SET_NAME, tls_threadName);

    try{
        pThrd->m_func();
    }catch(const Exception& e){
        fprintf(stderr, "exception caught in Thread %s\n", pThrd->m_name.c_str());
        fprintf(stderr, "reason: %s\n", e.what());
        fprintf(stderr, "stack trace: %s\n", e.stackTrace());
        abort();
    }catch(const std::exception& e){
        fprintf(stderr, "exception caught in Thread %s\n", pThrd->m_name.c_str());
        fprintf(stderr, "reason: %s\n", e.what());
        abort();
    }catch(...){
        fprintf(stderr, "exception caught in Thread %s\n", pThrd->m_name.c_str());
        throw; //rethrow
    }

    return nullptr;
}


} //namespace ybase
