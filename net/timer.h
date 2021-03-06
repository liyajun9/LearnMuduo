//
// Created by ralph on 4/11/20.
//

#ifndef LEARNMUDUO_TIMER_H
#define LEARNMUDUO_TIMER_H

#include <functional>
#include <atomic>
#include "../base/timestamp.h"
#include "alias.h"

namespace ynet {

class Timer {
NonCopyable(Timer)

public:
    Timer(TimerCallback cb, ybase::Timestamp when, double interval);

    void run() const { m_callback(); }
    void restart(ybase::Timestamp now);

    ybase::Timestamp getExpiration() const { return m_expiration; }
    double getInterval() const { return m_interval; }
    bool getRepeat() const { return m_repeat; }
    int64_t getSequence() const { return m_sequence; }

    static int64_t getNumcreated() { return s_numCreated.load(); }

private:
    const TimerCallback m_callback;
    const double m_interval;
    const bool m_repeat;
    const int64_t m_sequence;

    ybase::Timestamp m_expiration;

    static std::atomic<uint64_t> s_numCreated;

};

} //namespace ynet
#endif //LEARNMUDUO_TIMER_H
