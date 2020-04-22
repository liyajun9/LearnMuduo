//
// Created by ralph on 4/17/20.
//

#ifndef LEARNMUDUO_ACCEPTOR_H
#define LEARNMUDUO_ACCEPTOR_H

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

    void setNewConnectionCallback(const NewConnectionCallback& cb) { m_NewConnectionCb = cb; } //set by owner
    bool getListening() const { return m_listening; }

private:
    void handleRead();

    bool m_listening;
    int m_idleFd;

    Socket m_socket;
    Channel m_channel;
    NewConnectionCallback m_NewConnectionCb;
    EventLoop* m_ownerLoop;
};

} //namespace ynet
#endif //LEARNMUDUO_ACCEPTOR_H
