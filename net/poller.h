//
// Created by ralph on 4/11/20.
//

#ifndef LEARNMUDUO_POLLER_H
#define LEARNMUDUO_POLLER_H

#include <vector>
#include <map>
#include "channel.h"
#include "../base/timestamp.h"
#include "alias.h"

namespace ynet {
class EventLoop;

class Poller {
NonCopyable(Poller)


public:
    explicit Poller(EventLoop* loop);
    ~Poller();

    virtual ybase::Timestamp poll(int timeoutMs, std::vector<Channel*>& activeChannels) = 0; //activeChannels = have events occurred
    virtual void updateChannel(Channel* channel) = 0;   //adding or modifying channel
    virtual void removeChannel(Channel* channel) = 0;   //removing channel
    virtual bool hasChannel(Channel* channel) const;

protected:
    virtual void fillActiveChannels(int numEvents, std::vector<Channel*>& activeChannels) const = 0;

    void assertInCurrentThread();

protected:
    std::map<int, Channel*> m_channelMap; //all updated(added) channels
    EventLoop* m_ownerLoop;
};

} //namespace ynet
#endif //LEARNMUDUO_POLLER_H
