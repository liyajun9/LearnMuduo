//
// Created by ralph on 4/17/20.
//

#include <fcntl.h>
#include <unistd.h>
#include "acceptor.h"
#include "eventLoop.h"
#include "../base/logging.h"

namespace ynet{


Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort)
: m_ownerLoop(loop)
, m_socket(SocketUtils::createNonblockingOrDie(listenAddr.family()))
, m_channel(loop, m_socket.getFd())
, m_listening(false)
, m_idleFd(::open("/dev/null", O_RDONLY | O_CLOEXEC)){
    assert(m_idleFd >= 0);
    m_socket.setReuseAddr(true);
    m_socket.setReusePort(reusePort);
    m_socket.bind(listenAddr);
    m_channel.setReadCallback([this](ybase::Timestamp timestamp){
        this->handleRead();
    });
}

Acceptor::~Acceptor() {
    m_channel.disableAll();
    m_channel.remove();
    ::close(m_idleFd);
}

void Acceptor::listen() {
    m_ownerLoop->assertInCurrentThread();
    m_listening = true;
    SocketUtils::listenOrDie(m_socket.getFd());
    m_channel.enableRead();
}

void Acceptor::handleRead() {
    m_ownerLoop->assertInCurrentThread();
    InetAddress peerAddr;

    int connfd = m_socket.accept(&peerAddr);
    if(connfd >= 0){
        if(m_NewConnectionCb) m_NewConnectionCb(m_socket.getFd(), peerAddr);
        else SocketUtils::close(connfd);
    }else{
        LOG_SYSERR << "in Acceptor::handleRead";
        if(errno == EMFILE){ //fd is exhausted
            ::close(m_idleFd);
            m_idleFd = ::accept(m_socket.getFd(), NULL, NULL);
            ::close(m_idleFd);
            m_idleFd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}

} //namespace ynet