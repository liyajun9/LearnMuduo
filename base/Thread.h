//
// Created by ralph on 4/3/20.
//

#ifndef LINUXSERVER_THREAD_H
#define LINUXSERVER_THREAD_H

#include <functional>
#include <atomic>

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
    pid_t tid() const { return m_tid; }
    const std::string& name() const { return m_name; }

private:
    void  setDefaultName();

    bool        m_started;
    bool        m_joined;
    pthread_t   m_pthreadId;
    pid_t       m_tid;
    ThreadFunc  m_func;
    std::string m_name;

    static std::atomic<int> numCreated;
};

} //ybase
#endif //LINUXSERVER_THREAD_H
