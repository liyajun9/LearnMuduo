//
// Created by ralph on 4/11/20.
//

#include "poller.h"
#include "../base/logging.h"
#include "eventLoop.h"

namespace ynet{


Poller::Poller(EventLoop *loop)
: m_ownerLoop(loop){

}

Poller::~Poller() {

}

ybase::Timestamp Poller::poll(int timeoutMs, std::vector<Channel *>& activeChannels) {
    int numEvents = ::poll(&(*m_pollfdList.begin()), m_pollfdList.size(), timeoutMs);
    ybase::Timestamp now(ybase::Timestamp::now());
    if(numEvents > 0){
        LOG_TRACE << "Poller::poll" << numEvents << " events happened";
        fillActiveChannels(numEvents, activeChannels);
    }else if(numEvents == 0){
        LOG_TRACE << "Poller::poll nothing happened in " << timeoutMs << "ms";
    }else{
        LOG_SYSERR << "Poller::poll";
    }
    return now;
}

void Poller::removeChannel(Channel *channel) {
    assertInCurrentThread();
    LOG_TRACE << "fd = " << channel->getFd();
    assert(m_channelMap.find(channel->getFd()) != m_channelMap.end());
    assert(m_channelMap[channel->getFd()] == channel);
    assert(channel->isNoneEvent());

    int index = channel->getIndex();
    assert(index >= 0 && index < m_pollfdList.size());
    const struct pollfd& pfd = m_pollfdList[index];
    assert(pfd.fd == -channel->getFd() - 1 && pfd.events == channel->getEvents());
    size_t n = m_channelMap.erase(channel->getFd());
    assert(n == 1); (void)n;

    if(index == m_pollfdList.size() - 1){ //last element, directly remove
        m_pollfdList.pop_back();
    }else{ //not the last element, move it to last position, then remove
        int fdOfLastEle = m_pollfdList[m_pollfdList.size() - 1].fd; //used to set it's index
        std::iter_swap(m_pollfdList.begin() + index, m_pollfdList.end() - 1);
        if(fdOfLastEle < 0)
            fdOfLastEle = -fdOfLastEle-1;
        m_channelMap[fdOfLastEle]->setIndex(index);
        m_pollfdList.pop_back();
    }
}

void Poller::updateChannel(Channel *channel) {
    assertInCurrentThread(); //avoid adding to a different EventLoop
    LOG_TRACE << "Poller::update fd=" << channel->getFd() << " events=" << channel->getEvents();
    if(channel->getIndex() < 0){
        assert(m_channelMap.end() == m_channelMap.find(channel->getFd()));

        //adding pollfd to be polled
        Pollfd pollfd = {};
        pollfd.fd = channel->isNoneEvent() ? -1 : channel->getFd();
        pollfd.events = static_cast<short>(channel->getEvents());
        pollfd.revents = 0;
        m_pollfdList.push_back(pollfd);
        channel->setIndex(static_cast<int>(m_pollfdList.size() - 1));
        m_channelMap[channel->getFd()] = channel;
    }else{
        assert(m_channelMap.end() != m_channelMap.find(channel->getFd()));
        assert(channel == m_channelMap[channel->getFd()]);
        int index = channel->getIndex();
        assert(index >= 0 && index < m_pollfdList.size());
        Pollfd& pollfd = m_pollfdList[index];
        assert(channel->getFd() == pollfd.fd || pollfd.fd == -1);

        //update pollfd to be polled
        if(channel->isNoneEvent()){
            pollfd.fd = -channel->getFd()-1; //ignore none event channel, to avoid poll. use -fd-1 to save original info.
        }
        pollfd.events = static_cast<short>(channel->getEvents());
        pollfd.revents = 0;
    }
}

void Poller::fillActiveChannels(int numEvents, std::vector<Channel *>& activeChannels) const {
    for(auto it = m_pollfdList.begin(); numEvents > 0 && it != m_pollfdList.end(); ++it){
        if(it->revents > 0){
            --numEvents;
            //update channel
            auto iter = m_channelMap.find(it->fd);
            if(iter != m_channelMap.end()){
                iter->second->setCurrEvents(it->revents);
                activeChannels.push_back(iter->second);
            }
        }
    }
}

void Poller::assertInCurrentThread() {
    m_ownerLoop->assertInCurrentThread();
}


} //namespace ynet