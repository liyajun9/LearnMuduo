//
// Created by ralph on 4/17/20.
//

#include "tcpServer.h"
#include "inetAddress.h"
#include "eventLoop.h"
#include "acceptor.h"
#include "../base/logging.h"
#include "tcpConnection.h"

namespace ynet{


TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, std::string name, bool reusePort )
: m_loop(loop)
, m_ipPort(listenAddr.toIpPort())
, m_serverName(std::move(name))
, m_acceptor(std::unique_ptr<Acceptor>(new Acceptor(loop, listenAddr,reusePort)))
, m_connUpOrDownCb(defaultEstablishedCallback)
, m_messageCb(defaultMessageCallback)
, m_nextConnId(1)
, m_started(false){
    m_acceptor->setConnEstablishedCallback([this](int sockfd, const InetAddress &peerAddr){
        this->newConneciton(sockfd, peerAddr);
    });
}

TcpServer::~TcpServer(){

}

void TcpServer::start() {

}

void TcpServer::defaultEstablishedCallback(const TcpConnectionPtr& conn) {

}

void TcpServer::defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buf, ybase::Timestamp) {

}

void TcpServer::newConneciton(int sockfd, const InetAddress &peerAddr) {
    m_loop->assertInCurrentThread();

    //connection name
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", m_nextConnId++);
    std::string connName = m_serverName + buf;

    LOG_INFO << "TcpServer::newConnection [" << m_serverName << "] - new connection [" << connName << "] from " << peerAddr.toIpPort();
    InetAddress localAddr(SocketUtils::getLocalAddr(sockfd));
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(m_loop, connName, sockfd, localAddr, peerAddr);
    m_connectionMap[connName] = conn;
    conn->setConnUpOrDownCallback(m_connUpOrDownCb);
    conn->setMessageCallback(m_messageCb);
    conn->connectionEstablished();
}


} //namespace ynet
