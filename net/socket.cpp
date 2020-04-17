//
// Created by ralph on 4/17/20.
//

#include "socket.h"
#include "../base/logging.h"

namespace ynet{


Socket::Socket(int sockfd)
: m_sockfd(sockfd){

}

Socket::~Socket() {
    SocketUtils::close(m_sockfd);
}

void Socket::bind(const InetAddress &localaddr) {
    SocketUtils::bindOrDie(m_sockfd, localaddr.getSockAddr());
}

void Socket::listen() {
    SocketUtils::listenOrDie(m_sockfd);
}

int Socket::accept(InetAddress *peerAddr) {
    struct sockaddr_in6 addr;
    ybase::memZero(&addr, sizeof(addr));
    int connfd = SocketUtils::accept(m_sockfd, &addr);
    if(connfd >= 0){
        peerAddr->setSockAddInet6(addr);
    }
    return connfd;
}

void Socket::shutdownWrite() {
    SocketUtils::shutdownWrite(m_sockfd);
}

void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof(optval)));
}

void Socket::setReusePort(bool on) {
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof(optval)));
    if(ret < 0 && on){
        LOG_SYSERR << "SO_REUSEPORT failed";
    }
}

void Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof(optval)));
}


} //namespace ynet