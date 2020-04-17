//
// Created by ralph on 4/17/20.
//

#ifndef LINUXSERVER_TCPCONNECTION_H
#define LINUXSERVER_TCPCONNECTION_H


#include "socket.h"
#include "alias.h"
#include "buffer.h"
#include <memory>

namespace ynet {

class EventLoop;
class Channel;
class TcpConnection : public std::enable_shared_from_this<TcpConnection>{
NonCopyable(TcpConnection)
enum StateE {
    CONNECTING,
    CONNECTED,
    DISCONNECTING,
    DISCONNECTED
};

public:
    TcpConnection(EventLoop* loop, std::string name, int sockfd, const InetAddress& localAddr, const InetAddress& peerAddr);
    ~TcpConnection();

    void setConnUpOrDownCallback(ConnUpOrDownCallback cb) { m_connUpOrDownCb = std::move(cb); }
    void setCloseCallback(CloseCallback cb) { m_closeCb = std::move(cb); }
    void setMessageCallback(MessageCallback cb) { m_messageCb = std::move(cb); }

    void connectionEstablished();//used to change state
    void connectionDestroyed();

    void setState(StateE state) { m_state = state; }
    StateE getState() const { return m_state; }

private:
    void handleRead(ybase::Timestamp recvTime);
    void handleWrite();
    void handleClose();
    void handleError();

private:

    EventLoop* m_loop;
    std::string m_connName;
    StateE m_state;

    //connection info
    std::unique_ptr<Socket> m_socket;
    std::unique_ptr<Channel> m_channel;
    InetAddress m_localAddr;
    InetAddress m_peerAddr;

    //Input Buffer & Output Buffer
    Buffer m_inputBuf;
    Buffer m_outputBuf;

    ConnUpOrDownCallback m_connUpOrDownCb;
    CloseCallback m_closeCb;
    MessageCallback m_messageCb;
};

} //namespace ynet

#endif //LINUXSERVER_TCPCONNECTION_H
