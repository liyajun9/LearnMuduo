//
// Created by ralph on 4/21/20.
//

#include "epollPoller.h"
#include <sys/epoll.h>
#include "../alias.h"
#include "../../base/logging.h"

namespace ynet{


EpollPoller::EpollPoller(EventLoop *loop)
: Poller(loop)
, m_epollFd(::epoll_create(EPOLL_CLOEXEC))
, m_EpollEventList(s_InitEventListSize){
    if(m_epollFd < 0)
        LOG_SYSFATAL << "EpollPoller::EpollPoller";
}

EpollPoller::~EpollPoller(){
    ::close(m_epollFd);
}

ybase::Timestamp EpollPoller::poll(int timeoutMs, std::vector<Channel *> &activeChannels) {
    int numEvents = ::epoll_wait(m_epollFd, &m_EpollEventList[0], m_EpollEventList.size(), timeoutMs);
    int savedErrno = errno;
    ybase::Timestamp now(ybase::Timestamp::now());
    if(numEvents > 0){
        LOG_TRACE << numEvents << " events happended";
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == m_EpollEventList.size()) //in case of full, double size
            m_EpollEventList.resize(m_EpollEventList.size() * 2);
    }else if(numEvents == 0){
        LOG_TRACE << "nothing happened, just timeout";
    }else{
        if(savedErrno != EINTR){
            errno = savedErrno;
            LOG_SYSERR << "EpollPoller::poll()";
        }
    }
    return now;
}

void EpollPoller::updateChannel(Channel *channel) {
    Poller::assertInCurrentThread();
    const int index = channel->getIndex();
    LOG_TRACE << "fd = " << channel->getFd() << " events = " << channel->getEvents();
    if(index == s_New || index  == s_Deleted){ //new or deleted: add
        int fd = channel->getFd();
        if(index  == s_New){ //add
            assert(m_channelMap.find(fd) == m_channelMap.end());
            m_channelMap[fd] = channel;
        }else{
            assert(m_channelMap.find(fd) != m_channelMap.end());
            assert(m_channelMap[fd] == channel);
        }

        channel->setIndex(s_Added);
        update(EPOLL_CTL_ADD, channel);
    }else{ //exist fd: modify
        int fd = channel->getFd();
        assert(m_channelMap.find(fd) != m_channelMap.end());
        assert(m_channelMap[fd] == channel);
        assert(index == s_Added);
        if(channel->isNoneEvent()){
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(s_Deleted);
        }else{
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel *channel) {
    Poller::assertInCurrentThread();
    int fd = channel->getFd();
    LOG_TRACE << "fd = " << fd;
    assert(m_channelMap.find(fd) != m_channelMap.end());
    assert(m_channelMap[fd] == channel);
    assert(channel->isNoneEvent());

    int index = channel->getIndex();
    assert(index == s_Added || index == s_Deleted);
    size_t n = m_channelMap.erase(fd);
    assert(n == 1);

    if(index == s_Added){
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(s_New);
}

void EpollPoller::fillActiveChannels(int numEvents, std::vector<Channel *> &activeChannels) const {
    assert(numEvents <= m_EpollEventList.size());
    for(int  i = 0; i< numEvents; ++i){
        Channel* channel = reinterpret_cast<Channel*>(m_EpollEventList[i].data.ptr); //channle* in data.ptr
        channel->setCurrEvents(m_EpollEventList[i].events);
        activeChannels.push_back(channel);
    }
}

void EpollPoller::update(int operation, Channel *channel) {
    EpollEvent event;
    ybase::memZero(&event, sizeof(event));
    event.events = channel->getEvents();
    event.data.ptr = channel;
    int fd = channel->getFd();
    LOG_TRACE << "epoll_ctl op = " << operationToString((operation))
    << " fd = " << fd << " event = { " << channel->getEvents() << "}";

    if(::epoll_ctl(m_epollFd, operation, fd, &event) < 0){
        if(operation == EPOLL_CTL_DEL)
            LOG_SYSERR << "epoll_ctl op = " << operationToString(operation) << " fd = " << fd;
        else
            LOG_SYSFATAL << "epoll_ctl op = " << operationToString(operation) << " fd = " << fd;
    }
}

const char* EpollPoller::operationToString(int op)
{
    switch (op)
    {
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            assert(false && "ERROR op");
            return "Unknown Operation";
    }
}

}//namespace ynet