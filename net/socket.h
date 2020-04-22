//
// Created by ralph on 4/17/20.
//

#ifndef LEARNMUDUO_SOCKET_H
#define LEARNMUDUO_SOCKET_H

#include <netinet/tcp.h>
#include "inetAddress.h"

namespace ynet {

class Socket {
NonCopyable(Socket)
public:
    explicit Socket(int sockfd);
    ~Socket();

    void bind(const InetAddress& localaddr);
    void listen();
    int accept(InetAddress* peerAddr);
    void shutdownWrite();

    void setTcpNoDelay(bool on); //forbidden Nagle algo. to improve latency
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

    int getFd() const {return m_sockfd; }

private:
    const int m_sockfd;
};

} //namespace ynet


#endif //LEARNMUDUO_SOCKET_H
