//
// Created by ralph on 4/11/20.
//

#include "channel.h"
#include "eventLoop.h"
#include "../base/logging.h"

namespace ynet{


Channel::Channel(EventLoop *loop, int fd)
: m_ownerLoop(loop)
, m_fd(fd)
, m_events(NoneEvent)
, m_currEvents(NoneEvent)
, m_index(-1){

}

void Channel::handleEvents() {
    if(m_currEvents & POLLNVAL){
        LOG_WARN << "Channel::handle_events() POLLNVAL";
    }
    if(m_currEvents & (POLLIN | POLLPRI)){
        LOG_TRACE << "Channel::handle_events() POLLIN or POLLPRI";
        if(m_readCb) m_readCb();
    }
    if(m_currEvents & POLLOUT){
        LOG_TRACE << "Channel::handle_events() POLLOUT";
        if(m_writeCb) m_writeCb();
    }
    if(m_currEvents & POLLERR){
        LOG_TRACE << "Channel::handle_events() POLLERR";
        if(m_errorCb) m_errorCb();
    }
}

void Channel::updateChannel() {
    m_ownerLoop->updateChannel(this);
}

} //namespace ynet