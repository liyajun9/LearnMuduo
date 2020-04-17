//
// Created by ralph on 4/17/20.
//

#ifndef LINUXSERVER_TCPSERVER_H
#define LINUXSERVER_TCPSERVER_H

#include <memory>
#include <map>
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

    void start();
    void setConnUpOrDownCallback(ConnUpOrDownCallback cb) { m_connUpOrDownCb = std::move(cb); }
    void setMessageCallback(MessageCallback cb) { m_messageCb = std::move(cb); }

private:
    void newConneciton(int sockfd, const InetAddress& peerAddr);

    static void defaultEstablishedCallback(const TcpConnectionPtr& conn);
    static void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf, ybase::Timestamp);

private:
    EventLoop* m_loop;
    bool m_started;
    int m_nextConnId;
    const std::string m_serverName;
    const std::string m_ipPort;

    std::unique_ptr<Acceptor> m_acceptor;
    std::map<std::string, TcpConnectionPtr> m_connectionMap;

    ConnUpOrDownCallback m_connUpOrDownCb;
    MessageCallback m_messageCb;
};

} //namespace ynet

#endif //LINUXSERVER_TCPSERVER_H
