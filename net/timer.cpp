//
// Created by ralph on 4/11/20.
//

#include "timer.h"

namespace ynet{
std::atomic<uint64_t> Timer::s_numCreated(0);

Timer::Timer(TimerCallback cb, ybase::Timestamp when, double interval)
: m_callback(cb)
, m_expiration(when)
, m_interval(interval)
, m_repeat(interval > 0.0)
, m_sequence(++s_numCreated){

}

void Timer::restart(ybase::Timestamp now) {
    if(m_repeat)
        m_expiration = addTime(now, m_interval);
    else
        m_expiration = ybase::Timestamp::invalid();
}

} //namespace ynet