//
// Created by ralph on 4/3/20.
//

#ifndef LINUXSERVER_EVENTLOOP_H
#define LINUXSERVER_EVENTLOOP_H

namespace ynet {

class EventLoop {
    NonCopyable(EventLoop)

private:
    bool m_looping;
    const pid_t threadId_;
};

}//namepace ynet


#endif //LINUXSERVER_EVENTLOOP_H
