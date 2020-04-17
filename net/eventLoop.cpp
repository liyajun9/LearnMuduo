//
// Created by ralph on 4/3/20.
//

#include <poll.h>
#include "eventLoop.h"
#include "../base/logging.h"
#include "poller.h"
#include "../base/systemUtils.h"
#include "socketUtils.h"

namespace ynet{
thread_local EventLoop* pLoopInThread = nullptr;

EventLoop* getEventLoopOfCurrentThread(){
    return pLoopInThread;
}

EventLoop::EventLoop()
: m_looping(false)
, m_threadId(ybase::ThreadUtils::getTid())
, m_quit(false)
, m_timerQueue(new TimerQueue(this))
, m_asyncTaskFd(ybase::SystemUtils::createEventfd())
, m_asyncTaskChannel(new Channel(this, m_asyncTaskFd))
, m_executingAsyncTask(false){
    LOG_TRACE << "EventLoop " << this << "created in thread " << m_threadId;
    if(pLoopInThread){
        LOG_FATAL << "Another EventLoop " << pLoopInThread << "exists in this thread " << m_threadId;
    }else{
        pLoopInThread = this;
    }

    //register wakeupFd
    m_asyncTaskChannel->setReadCallback([this](){
        this->resetAsyncTaskEvent();
    });
    m_asyncTaskChannel->enableRead();
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
    LOG_TRACE << "EventLoop " << this << "start looping";

    while(!m_quit){
        m_activeIOChannels.clear();
        m_poller->poll(1000, m_activeIOChannels);
        for(auto channel : m_activeIOChannels){
            channel->handleEvents();
        }
        executeAsyncTasks();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    m_looping = false;
}

void EventLoop::assertInCurrentThread() {
    if(!isInLoopThread()){
        LOG_FATAL << "EventLoop " << this << " is in thread " << m_threadId << " not in current thread" << ybase::ThreadUtils::getTid();
    }
}

void EventLoop::updateChannel(Channel *channel) {
    assert(channel->getOwnerLoop() == this);
    assertInCurrentThread();
    m_poller->updateChannel(channel);
}

TimerId EventLoop::runAt(const ybase::Timestamp &time, TimerCallback &cb) {
    return m_timerQueue->addTimer_mt(cb, time, 0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback &cb) {
    ybase::Timestamp time(addTime(ybase::Timestamp::now(), delay));
    return m_timerQueue->addTimer_mt(cb, time, 0);
}

TimerId EventLoop::runEvery(double interval, TimerCallback &cb) {
    ybase::Timestamp time(addTime(ybase::Timestamp::now(), interval));
    return m_timerQueue->addTimer_mt(cb, time, interval);
}

void EventLoop::postTask_mt(AsyncTask &cb) {
    if(isInLoopThread())
        cb();
    else
        queueTask(cb);
}

void EventLoop::queueTask(AsyncTask &cb) {
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_asyncTaskList.push_back(std::move(cb));
    }

    //setAsyncTaskEvent: other threads or during functor
    if(!isInLoopThread() || m_executingAsyncTask)
        setAsyncTaskEvent();
}

void EventLoop::executeAsyncTasks() {
    //copy then execute
    std::vector<AsyncTask> taskList;
    m_executingAsyncTask = true;

    {
        std::lock_guard<std::mutex> lock(m_mtx);
        taskList.swap(m_asyncTaskList);
    }

    for(const auto& task : taskList)
        task();

    m_executingAsyncTask = false;
}

void EventLoop::quit_mt() {
    m_quit = true;
    if(!isInLoopThread())
        setAsyncTaskEvent();
}

void EventLoop::setAsyncTaskEvent() {
    uint64_t one = 1;
    ssize_t n = SocketUtils::write(m_asyncTaskFd, &one, sizeof(one));
    if(n != sizeof(one))
        LOG_ERROR << "EventLoop::setAsyncTaskEvent() writes " << n << " bytes instead of 8";
}

void EventLoop::resetAsyncTaskEvent() {
    uint64_t one = 1;
    ssize_t n = SocketUtils::read(m_asyncTaskFd, &one, sizeof(one));
    if(n != sizeof(one))
        LOG_ERROR << "EventLoop::resetAsyncTaskEvent() reads " << n << " bytes instead of 8";
}

void EventLoop::removeChannel(Channel *channel) {
    assert(channel->getOwnerLoop() == this);
    assertInCurrentThread();

    m_poller->removeChannel(channel);
}

} //namespace ynet