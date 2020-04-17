//
// Created by ralph on 4/3/20.
//

#ifndef LINUXSERVER_EVENTLOOP_H
#define LINUXSERVER_EVENTLOOP_H

#include <memory>
#include <vector>
#include <mutex>
#include "channel.h"
#include "timerId.h"
#include "../base/timestamp.h"
#include "timer.h"
#include "timerQueue.h"
#include "../base/threadUtils.h"
#include "alias.h"

namespace ynet {

class EventLoop {
    NonCopyable(EventLoop)

public:
    EventLoop();
    ~EventLoop();

    /* start looping. loop body.
     *
     */
    void loop();
    void quit_mt(); //quit loop, the suffix '_mt' means it's thread safe
    void updateChannel(Channel* channel);   //update channel for poll
    void removeChannel(Channel* channel);   //remove channel from poll

    /* set timer
     *
     */
    TimerId runAt(const ybase::Timestamp& time, TimerCallback& cb);
    TimerId runAfter(double delay, TimerCallback& cb);
    TimerId runEvery(double interval, TimerCallback& cb);

    /* post tasks
     * 1.invoke from loop thread: execute task immediately
     * 2.invoke from other threads: produce event on wakeupfd
     */
    void postTask_mt(AsyncTask &cb); //current thread: execute right now, other thread: adding to queue and wake up loop

    bool isLooping() const { return m_looping; }
    pid_t getThreadId() const { return m_threadId; }
    void assertInCurrentThread();

private:
    void queueTask(AsyncTask &cb);
    void executeAsyncTasks();
    void setAsyncTaskEvent();       //write to m_asyncTaskFd
    void resetAsyncTaskEvent();     //read from m_asyncTaskFd

    bool isInLoopThread() const { return m_threadId != ybase::ThreadUtils::getTid(); }

private:
    bool m_looping;
    std::atomic<bool> m_quit;
    const pid_t m_threadId;

    std::unique_ptr<Poller> m_poller;

    //(active)IOChannels
    std::vector<Channel*> m_activeIOChannels;

    //timers
    std::unique_ptr<TimerQueue> m_timerQueue;

    //asynchronise task
    int m_asyncTaskFd;
    std::unique_ptr<Channel> m_asyncTaskChannel;
    std::mutex m_mtx;
    std::vector<AsyncTask> m_asyncTaskList;
    bool m_executingAsyncTask;
};

EventLoop* getEventLoopOfCurrentThread();
}//namepace ynet


#endif //LINUXSERVER_EVENTLOOP_H
