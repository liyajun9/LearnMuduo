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
, m_index(-1)
, m_tied(false){

}

void Channel::handleEvents() {
    std::shared_ptr<void> connPtr;
    if(m_tied){
        connPtr = m_tie.lock();
        if(!connPtr)
            return;
    }
    if(m_currEvents & POLLNVAL){
        LOG_WARN << "Channel::handle_events() POLLNVAL";
    }
    if((m_currEvents & POLLHUP) && !(m_currEvents & POLLIN)){
        LOG_WARN << "Channel::handle_events() POLLHUP";
        if(m_closeCb) m_closeCb();
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

void Channel::update() {
    m_ownerLoop->updateChannel(this);
}

void Channel::remove() {
    assert(isNoneEvent());
    m_ownerLoop->removeChannel(this);
}

void Channel::tie(const std::shared_ptr<void>& ptr) {
    m_tie = ptr;
    m_tied = true;
}

} //namespace ynet