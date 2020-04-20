//
// Created by ralph on 4/17/20.
//

#ifndef LINUXSERVER_TCPSERVER_H
#define LINUXSERVER_TCPSERVER_H

#include <memory>
#include <map>
#include <atomic>
#include "alias.h"

namespace ynet {

class EventLoop;
class Acceptor;
class EventLoopThreadPool;

class TcpServer {
NonCopyable(TcpServer)
public:
    TcpServer(EventLoop* loop, const InetAddress& listenAddr, std::string name, bool reusePort = false);
    ~TcpServer();

    void setThreadNum(int numThreads);

    void start();
    void setConnectionChangeCallback(ConnectionChangeCallback cb) { m_connectionChangeCb = std::move(cb); }
    void setMessageCallback(MessageCallback cb) { m_messageCb = std::move(cb); }
    void setWriteCompleteCallback(WriteCompleteCallback cb) { m_writeCompleteCb = std::move(cb); }

private:
    void newConnection(int sockFd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    static void defaultConnectionChangedCallback(const TcpConnectionPtr& conn);
    static void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf, ybase::Timestamp);

private:
    std::atomic<bool> m_started;
    int m_nextConnId;
    const std::string m_serverName;
    const std::string m_ipPort;

    EventLoop* m_loop; //only for acceptor
    std::shared_ptr<EventLoopThreadPool> m_loopThreadPool;

    std::unique_ptr<Acceptor> m_acceptor;
    std::map<std::string, TcpConnectionPtr> m_connectionMap;

    LoopThreadInitCallback m_loopThreadInitCb;
    ConnectionChangeCallback m_connectionChangeCb;
    MessageCallback m_messageCb;
    WriteCompleteCallback m_writeCompleteCb;
};

} //namespace ynet

#endif //LINUXSERVER_TCPSERVER_H
