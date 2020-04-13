//
// Created by ralph on 4/13/20.
//

#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include "systemUtils.h"
#include "logging.h"

namespace ybase{

int SystemUtils::createTimerfd() {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0){
        LOG_SYSFATAL << "Failed in timerfd_create";
    }
    return timerfd;
}

int SystemUtils::createEventfd() {
    int eventfd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK | EFD_SEMAPHORE);
    if(eventfd < 0){
        LOG_SYSFATAL << "Failed in eventfd";
    }
    return eventfd;
}

} //namespace ybase