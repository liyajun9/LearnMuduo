//
// Created by ralph on 4/11/20.
//

#ifndef LINUXSERVER_TIMERQUEUE_H
#define LINUXSERVER_TIMERQUEUE_H

#include <memory>
#include <set>
#include <vector>
#include "../base/timestamp.h"
#include "channel.h"
#include "timer.h"
#include "timerId.h"

namespace ynet {

class TimerQueue {
public:
    using Entry = std::pair<ybase::Timestamp, Timer*>;
    using TimerList = std::set<Entry>;

    using TimerIdEntry = std::pair<Timer*, int64_t>; //raw pointers of Timer, so must operator together with TimerList.
    using TimerIdList = std::set<TimerIdEntry>;

public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(Timer::TimerCallback& cb, ybase::Timestamp when, double interval);
    void cancelTimer(int64_t timerSequence);

    void addTimerInLoop(Timer *timer);
    void cancelInLoop(TimerId timerId);

private:
    void handleRead(); //run callback and reset expired timers
    std::vector<Entry> getExpired(ybase::Timestamp now); //get expired timers from m_timerList
    void reset(std::vector<Entry>& expired, ybase::Timestamp now); //reset repeat expired timers, update timerfd according to nearest expiration
    bool insert(Timer *timer); //insert into timerList, return whether it is the earliest timer to be expired

    static int createTimerfd();
    static struct timespec howMuchTimeFromNow(ybase::Timestamp when);
    static void readTimerfd(int timerfd, ybase::Timestamp now);
    static void resetTimerfd(int timerfd, ybase::Timestamp expiration);

private:
    const int m_timerfd;
    Channel m_timerfdChannel;
    TimerList m_timerList;   //timer list sorted by expiration

    //for cancel
    TimerIdList m_timerIdList;    //timerid list corresponding to timerList. It's used to get expiration so that we can construct Entry and erase it from timerList
    TimerIdList m_cancelingTimers;    //when we cancel timers that are executing callback, store it in this list and do not reset it again
    std::atomic<bool> m_callingExpiredTimers; //mark the timer that it is executing callback

    EventLoop* m_loop;
};

} //namespace ynet

#endif //LINUXSERVER_TIMERQUEUE_H
