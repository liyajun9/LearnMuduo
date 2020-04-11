//
// Created by ralph on 4/11/20.
//

#ifndef LINUXSERVER_POLLER_H
#define LINUXSERVER_POLLER_H

#include <vector>
#include <map>
#include "channel.h"
#include "../base/timestamp.h"

namespace ynet {
class EventLoop;

class Poller {
NonCopyable(Poller)

public:
    using Pollfd = struct pollfd;

public:
    explicit Poller(EventLoop* loop);
    ~Poller();

    ybase::Timestamp poll(int timeoutMs, std::vector<Channel*>& activeChannels); //activeChannels = have events occurred
    void updateChannel(Channel* channel); //adding or modifying channel

private:
    void fillActiveChannels(int numEvents, std::vector<Channel*>& activeChannels) const;

    void assertInCurrentThread();

private:
    std::vector<Pollfd> m_pollfdList; //pollfdList correspondign to m_channelMap

    std::map<int, Channel*> m_channelMap; //all updated(added) channels
    EventLoop* m_ownerLoop;
};

} //namespace ynet
#endif //LINUXSERVER_POLLER_H
