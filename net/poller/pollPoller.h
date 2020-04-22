//
// Created by ralph on 4/21/20.
//

#ifndef LEARNMUDUO_POLLPOLLER_H
#define LEARNMUDUO_POLLPOLLER_H

#include "../poller.h"

namespace ynet {

class EventLoop;
class PollPoller : public Poller{
NonCopyable(PollPoller)

public:
    explicit PollPoller(EventLoop* loop);
    ~PollPoller();

    ybase::Timestamp poll(int timeoutMs, std::vector<Channel*>& activeChannels) override ;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:
    void fillActiveChannels(int numEvents, std::vector<Channel*>& activeChannels) const override;

private:
    std::vector<Pollfd> m_pollFdList; //pollfdList correspondign to m_channelMap

};

} //namespace ynet

#endif //LEARNMUDUO_POLLPOLLER_H
