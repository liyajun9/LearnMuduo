//
// Created by ralph on 4/11/20.
//

#include "timerQueue.h"
#include "../base/logging.h"
#include <sys/timerfd.h>
#include <algorithm>
#include <search.h>
#include "eventLoop.h"

namespace ynet{


TimerQueue::TimerQueue(EventLoop *loop)
: m_loop(loop)
, m_timerfd(createTimerfd())
, m_timerfdChannel(loop, m_timerfd)
, m_callingExpiredTimers(false){
    auto cb = [this](){
        this->handleRead();
    };
    m_timerfdChannel.setReadCallback(cb);
    m_timerfdChannel.enableRead();
}

TimerQueue::~TimerQueue() {
    m_timerfdChannel.disableAll();
//    m_timerfdChannel.remove(); //To do
    ::close(m_timerfd);

    for(const Entry& timer : m_timerList)
        delete timer.second;
}

TimerId TimerQueue::addTimer(Timer::TimerCallback &cb, ybase::Timestamp when, double interval) {
    std::unique_ptr<Timer> timer(new Timer(std::move(cb), when, interval));
//    m_loop->runInLoop() //To do
    return {timer.get(), timer->getSequence()};
}

void TimerQueue::cancelTimer(int64_t timerSequence) {
//    m_loop->runInLoop(); //To do
}

int TimerQueue::createTimerfd() {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0){
        LOG_SYSFATAL << "Failed in timerfd_create";
    }
    return timerfd;
}

struct timespec TimerQueue::howMuchTimeFromNow(ybase::Timestamp when) {
    int64_t microSeconds = when.microSecondsSinceEpoch() - ybase::Timestamp::now().microSecondsSinceEpoch();
    if(microSeconds < 100)
        microSeconds = 100;

    struct timespec ts = {};
    ts.tv_sec = static_cast<time_t>(microSeconds / ybase::Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microSeconds % ybase::Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void TimerQueue::readTimerfd(int timerfd, ybase::Timestamp now) {
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
    LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
    if(n != sizeof(howmany))
        LOG_ERROR << "TimerQueue::handleRead() reads " << n << "bytes instead of 8";
}

void TimerQueue::resetTimerfd(int timerfd, ybase::Timestamp expiration) {
    struct itimerspec newValue = {};
    struct itimerspec oldValue = {};
    ybase::memZero(&newValue, sizeof(newValue));
    ybase::memZero(&oldValue, sizeof(oldValue));
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if(ret)
        LOG_SYSERR << "timerfd_settime() error";
}

void TimerQueue::handleRead() {
    m_loop->assertInCurrentThread();
    ybase::Timestamp now(ybase::Timestamp::now());
    readTimerfd(m_timerfd, now);

    std::vector<TimerQueue::Entry> expired = getExpired(now);

    m_callingExpiredTimers.store(true);
    m_cancelingTimers.clear();
    for(const TimerQueue::Entry& entry : expired){
        entry.second->run();
    }
    m_callingExpiredTimers.store(false);

    reset(expired, now);
}

vector<TimerQueue::Entry> TimerQueue::getExpired(ybase::Timestamp now) {
    assert(m_timerList.size() == m_timerIdList.size());
    std::vector<TimerQueue::Entry> expiredTimers;

    //remove expired timers from timerlist
    TimerQueue::Entry sentry(now, nullptr);
    auto it = m_timerList.lower_bound(sentry);
    assert(it == m_timerList.end() || it->first > now); // '>' not '>=' because unique_ptr is emtpy
    std::copy(m_timerList.begin(), it, std::back_inserter(expiredTimers));
    m_timerList.erase(m_timerList.begin(), it);

    //remove expired timers from activeTimers
    for(const TimerQueue::Entry& entry: expiredTimers){
        TimerIdEntry timer(entry.second, entry.second->getSequence());
        size_t n = m_timerIdList.erase(timer);
        assert(n == 1);
    }
    assert(m_timerList.size() == m_timerIdList.size());

    return expiredTimers;
}

void TimerQueue::reset(std::vector<TimerQueue::Entry>& expired, ybase::Timestamp now) {
    ybase::Timestamp nextExpiration;

    for(TimerQueue::Entry& entry : expired){
        TimerIdEntry timer(entry.second, entry.second->getSequence());
        //restart repeat timers
        if(entry.second->getRepeat() && m_cancelingTimers.find(timer) == m_cancelingTimers.end()){
            entry.second->restart(now);
            insert(entry.second);
        }
        //remove
        else{
            delete entry.second;
        }
    }

    if(!m_timerList.empty())
        nextExpiration = m_timerList.begin()->second->getExpiration();

    if(nextExpiration.valid())
        resetTimerfd(m_timerfd, nextExpiration);
}

bool TimerQueue::insert(Timer *timer) {
    m_loop->assertInCurrentThread();
    assert(m_timerList.size() == m_timerIdList.size());
    bool earlistExpire = false;
    ybase::Timestamp expire = timer->getExpiration();
    if(m_timerList.empty() || expire < m_timerList.begin()->first)
        earlistExpire = true;
    {
        std::pair<TimerIdList::iterator, bool> res = m_timerIdList.insert(TimerIdEntry(timer, timer->getSequence()));
        assert(res.second);
    }
    {
        std::pair<TimerList::iterator, bool> res = m_timerList.insert(TimerQueue::Entry(expire, timer));
        assert(res.second);
    }


    assert(m_timerList.size() == m_timerIdList.size());
    return earlistExpire;
}

void TimerQueue::addTimerInLoop(Timer *timer) {
    m_loop->assertInCurrentThread();
    ybase::Timestamp expiration = timer->getExpiration();
    bool earlistExpire = insert(timer);
    if(earlistExpire)
        resetTimerfd(m_timerfd, expiration);
}

void TimerQueue::cancelInLoop(TimerId timerId) {
    m_loop->assertInCurrentThread();
    assert(m_timerList.size() == m_timerIdList.size());
    TimerIdEntry timerIdEntry(timerId.m_timer, timerId.m_sequence);
    auto it = m_timerIdList.find(timerIdEntry);
    if(it != m_timerIdList.end()){
        size_t n = m_timerList.erase(Entry(it->first->getExpiration(), it->first));
        delete it->first;
        m_timerIdList.erase(it);
    }else if(m_callingExpiredTimers.load()){
        m_cancelingTimers.insert(timerIdEntry);
    }
    assert(m_timerList.size() == m_timerIdList.size());
}

} //namespace ynet