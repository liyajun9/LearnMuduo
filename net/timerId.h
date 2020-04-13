//
// Created by ralph on 4/13/20.
//

#ifndef LINUXSERVER_TIMERID_H
#define LINUXSERVER_TIMERID_H

#include <cstdint>

namespace ynet {

class Timer;
class TimerId {
public:
    TimerId()
    :m_timer(nullptr)
    ,m_sequence(0){}

    TimerId(Timer* timer, int64_t sequence)
    :m_timer(timer)
    ,m_sequence(sequence){}

    Timer* m_timer;
    uint64_t m_sequence;
};

} //namespace ynet


#endif //LINUXSERVER_TIMERID_H
