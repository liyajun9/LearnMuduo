//
// Created by ralph on 4/3/20.
//

#ifndef LINUXSERVER_THREAD_H
#define LINUXSERVER_THREAD_H

#include <functional>
#include <atomic>
#include "CountdownLatch.h"

namespace ybase{

class Thread {
NonCopyable(Thread)

public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc func, std::string name = std::string());
    ~Thread();

    void start();
    int join();

    bool started() const { return m_started; }
    pid_t getTid() const { return m_tid; }
    const std::string& getName() const { return m_name; }

private:
    void  setDefaultName();
    static void* run(void* arg);

    bool        m_started;
    bool        m_joined;
    pthread_t   m_pthreadId;
    pid_t       m_tid;
    ThreadFunc  m_func;
    std::string m_name;

    static std::atomic<int> numCreated; //record instances of Thread
    CountdownLatch m_latch;
};

} //ybase
#endif //LINUXSERVER_THREAD_H
