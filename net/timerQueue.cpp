//
// Created by ralph on 4/11/20.
//

#include "timerQueue.h"
#include "../base/logging.h"
#include <sys/timerfd.h>

namespace ynet{


TimerQueue::TimerQueue(EventLoop *loop)
: m_loop(loop)
, m_timerfd(createTimerfd())
, m_timerfdChannel(loop, m_timerfd){

}

TimerQueue::~TimerQueue() {

}

void TimerQueue::addTimer(const Timer::TimerCallback &cb, ybase::Timestamp when, double interval) {

}

void TimerQueue::cancelTimer(int64_t timerSequence) {

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

    struct timespec ts;
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
    struct itimerspec newValue;
    struct itimerspec oldValue;
    ybase::memZero(&newValue, sizeof(newValue));
    ybase::memZero(&oldValue, sizeof(oldValue));
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if(ret)
        LOG_SYSERR << "timerfd_settime() error";
}
} //namespace ynet