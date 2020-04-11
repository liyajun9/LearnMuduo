//
// Created by ralph on 4/3/20.
//

#ifndef LINUXSERVER_EVENTLOOP_H
#define LINUXSERVER_EVENTLOOP_H

#include <memory>
#include <vector>
#include "channel.h"

namespace ynet {

class EventLoop {
    NonCopyable(EventLoop)

public:
    EventLoop();
    ~EventLoop();

    void loop();
    void updateChannel(Channel* channel);

    void quit_mt() { m_quit = true; }
    bool isLooping() const { return m_looping; }
    pid_t getThreadId() const { return m_threadId; }

    void assertInCurrentThread();

private:
    bool m_looping;
    bool m_quit;
    const pid_t m_threadId;

    std::unique_ptr<Poller> m_poller;
    std::vector<Channel*> m_activeChannels;
};

EventLoop* getEventLoopOfCurrentThread();
}//namepace ynet


#endif //LINUXSERVER_EVENTLOOP_H
