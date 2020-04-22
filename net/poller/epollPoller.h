//
// Created by ralph on 4/21/20.
//

#ifndef LEARNMUDUO_EPOLLPOLLER_H
#define LEARNMUDUO_EPOLLPOLLER_H

#include <sys/epoll.h>
#include "../poller.h"

static_assert(EPOLLIN == POLLIN,        "epoll uses same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll uses same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll uses same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll uses same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll uses same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll uses same flag values as poll");

namespace ynet {

class EventLoop;
class EpollPoller : public Poller{
NonCopyable(EpollPoller)

public:
    explicit EpollPoller(EventLoop* loop);
    ~EpollPoller();

    ybase::Timestamp poll(int timeoutMs, std::vector<Channel*>& activeChannels) override ;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:
    void fillActiveChannels(int numEvents, std::vector<Channel*>& activeChannels) const override;
    void update(int operation, Channel* channel);
    static const char* operationToString(int op);

private:
    int m_epollFd;
    std::vector<EpollEvent> m_EpollEventList;

    static constexpr int s_InitEventListSize = 16;

    /* used by index to mark channel status (in channelMap)
     *
     */
    static constexpr int s_New = -1;
    static constexpr int s_Added = 1;
    static constexpr int s_Deleted = 2;
};

} //namespace ynet


#endif //LEARNMUDUO_EPOLLPOLLER_H
