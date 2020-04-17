//
// Created by ralph on 4/17/20.
//

#ifndef LINUXSERVER_ACCEPTOR_H
#define LINUXSERVER_ACCEPTOR_H

#include "socket.h"
#include "channel.h"
#include "alias.h"

namespace ynet {

class Acceptor {
NonCopyable(Acceptor)

public:
    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort);
    ~Acceptor();

    void listen();

    void setConnEstablishedCallback(const ConnEstablishedCallback& cb) { m_ConnEstablishedCb = cb; }
    bool getListening() const { return m_listening; }

private:
    void handleRead();

    bool m_listening;
    int m_idleFd;

    Socket m_socket;
    Channel m_channel;
    ConnEstablishedCallback m_ConnEstablishedCb;
    EventLoop* m_ownerLoop;
};

} //namespace ynet
#endif //LINUXSERVER_ACCEPTOR_H
