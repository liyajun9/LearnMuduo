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

Poller::~Poller() = default;

void Poller::assertInCurrentThread() {
    m_ownerLoop->assertInCurrentThread();
}

bool Poller::hasChannel(Channel* channel) const{
    m_ownerLoop->assertInCurrentThread();
    auto it = m_channelMap.find(channel->getFd());
    return it != m_channelMap.end() && it->second == channel;
}


} //namespace ynet