//
// Created by ralph on 4/11/20.
//

#ifndef LINUXSERVER_TIMERQUEUE_H
#define LINUXSERVER_TIMERQUEUE_H

#include <memory>
#include <set>
#include "../base/timestamp.h"
#include "channel.h"
#include "timer.h"

namespace ynet {

class TimerQueue {
public:
    using Entry = std::pair<ybase::Timestamp, std::unique_ptr<Timer>>;
    using TimerList = std::set<Entry>;

public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    void addTimer(const Timer::TimerCallback& cb, ybase::Timestamp when, double interval);
    void cancelTimer(int64_t timerSequence);

private:
    static int createTimerfd();
    static struct timespec howMuchTimeFromNow(ybase::Timestamp when);
    static void readTimerfd(int timerfd, ybase::Timestamp now);
    static void resetTimerfd(int timerfd, ybase::Timestamp expiration);

private:
    const int m_timerfd;
    Channel m_timerfdChannel;
    TimerList m_timerList;

    EventLoop* m_loop;
};

} //namespace ynet

#endif //LINUXSERVER_TIMERQUEUE_H
