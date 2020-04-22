//
// Created by ralph on 4/17/20.
//

#ifndef LEARNMUDUO_TCPCONNECTION_H
#define LEARNMUDUO_TCPCONNECTION_H


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

    void send_mt(const std::string& message);
    void send_mt(const void* message, int len);
    void send_mt(const ybase::StringPiece& message);
    void send_mt(Buffer* message);
    void shutdown_mt();
    void forceClose();
    void forceCloseWithDelay(double seconds);

    void setTcpNoDelay(bool on);
    void startRead();
    void stopRead();
    bool isReading() const { return m_reading; }

    void setConnectionChangeCallback(ConnectionChangeCallback cb) { m_connectionChangeCb = std::move(cb); }
    void setCloseCallback(CloseCallback cb) { m_closeCb = std::move(cb); }
    void setMessageCallback(MessageCallback cb) { m_messageCb = std::move(cb); }
    void setWriteCompleteCallback(WriteCompleteCallback cb) { m_writeCompleteCb = std::move(cb); }
    void setHighWaterMarkCallback(HighWaterMarkCallback cb) { m_highWaterMarkCb = std::move(cb); }

    void connectionEstablished();//used to change state
    void connectionDestroyed();

    void setState(StateE state) { m_state = state; }
    StateE getState() const { return m_state; }
    bool IsConnected() const { return m_state == CONNECTED; }
    bool IsDisconnected() const { return m_state == DISCONNECTED; }

    std::string getConnName() const { return m_connName; };
    EventLoop* getLoop() const { return m_loop; }
    InetAddress getLocalAddr() const { return m_localAddr; }
    InetAddress getPeerAddr() const { return m_peerAddr; }

    //advanced interface
    Buffer* recvBuffer() { return &m_recvBuf; }
    Buffer* sendBuffer() { return &m_sendBuf; }

private:
    /* pass to Channel::ReadEventCallback & EventCallback
     *
     */
    void handleRead(ybase::Timestamp recvTime);
    void handleWrite();
    void handleClose();
    void handleError();

    void sendInLoop(const std::string& message);
    void sendInLoop(const ybase::StringPiece& message);
    void sendInLoop(const void *message, size_t len);
    void shutdownInLoop(); //shutdown write
    void forceCloseInLoop();
    void startReadInLoop();
    void stopReadInLoop();

private:

    EventLoop* m_loop;
    std::string m_connName;
    StateE m_state;
    bool m_reading;

    //connection info
    std::unique_ptr<Socket> m_socket;
    std::unique_ptr<Channel> m_channel;
    InetAddress m_localAddr;
    InetAddress m_peerAddr;

    //recv Buffer & send Buffer
    Buffer m_recvBuf;
    Buffer m_sendBuf;

    ConnectionChangeCallback m_connectionChangeCb;
    CloseCallback m_closeCb;
    MessageCallback m_messageCb;
    WriteCompleteCallback m_writeCompleteCb; //on sendBuf is empty
    HighWaterMarkCallback m_highWaterMarkCb; //on sendBuf > highWaterMark

    static constexpr size_t s_highWaterMark = 64 * 1024 * 1024;
};

} //namespace ynet

#endif //LEARNMUDUO_TCPCONNECTION_H
