//
// Created by ralph on 4/17/20.
//

#include "tcpServer.h"
#include "inetAddress.h"
#include "eventLoop.h"
#include "acceptor.h"
#include "../base/logging.h"
#include "tcpConnection.h"
#include "eventLoopThreadPool.h"

namespace ynet{


TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, std::string name, bool reusePort )
: m_loop(loop)
, m_ipPort(listenAddr.toIpPort())
, m_serverName(std::move(name))
, m_loopThreadPool(new EventLoopThreadPool(m_loop, m_serverName))
, m_acceptor(std::unique_ptr<Acceptor>(new Acceptor(loop, listenAddr,reusePort)))
, m_connectionChangeCb(defaultConnectionChangedCallback)
, m_messageCb(defaultMessageCallback)
, m_nextConnId(1)
, m_started(false){
    m_acceptor->setNewConnectionCallback([this](int sockfd, const InetAddress &peerAddr) {
        this->newConnection(sockfd, peerAddr);
    });
}

TcpServer::~TcpServer(){
    m_loop->assertInCurrentThread();
    LOG_TRACE << "TcpServer::~TcpServer [" << m_serverName << "] destructing";

    for(auto& connection : m_connectionMap){
        TcpConnectionPtr conn(connection.second);
        connection.second.reset();
        conn->getLoop()->postTask_mt([&conn](){
            conn->connectionDestroyed();
        });
    }
}

void TcpServer::start() {
    if(!m_started.load()){
        m_loopThreadPool->start(m_loopThreadInitCb);

        //listen in baseLoop
        assert(!m_acceptor->getListening());
        m_loop->postTask_mt([this](){
            this->m_acceptor->listen();
        });
    }
}

void TcpServer::defaultConnectionChangedCallback(const TcpConnectionPtr& conn) {
    LOG_TRACE << conn->getLocalAddr().toIpPort() << " -> "
              << conn->getPeerAddr().toIpPort() << " is "
              << (conn->isConnected() ? "UP" : "DOWN");
}

void TcpServer::defaultMessageCallback(const TcpConnectionPtr &conn, Buffer *buf, ybase::Timestamp) {
    buf->retrieveAll();
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    m_loop->assertInCurrentThread();

    EventLoop* ioLoop = m_loopThreadPool->getNextLoop(); //select a ioLoop from threadPool

    //connection name
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", m_nextConnId++);
    std::string connName = m_serverName + buf;

    LOG_INFO << "TcpServer::newConnection [" << m_serverName << "] - new connection [" << connName << "] from " << peerAddr.toIpPort();
    InetAddress localAddr(SocketUtils::getLocalAddr(sockfd));
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioLoop, connName, sockfd, localAddr, peerAddr);
    m_connectionMap[connName] = conn;
    conn->setConnectionChangeCallback(m_connectionChangeCb);
    conn->setMessageCallback(m_messageCb);
    conn->setWriteCompleteCallback(m_writeCompleteCb);
    conn->connectionEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
    m_loop->postTask_mt([this, &conn](){
        this->removeConnectionInLoop(conn);
    });
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    m_loop->assertInCurrentThread();
    LOG_INFO << "TcpServer::removeConnectionInLoop [" << m_serverName
    << "] - connection " << conn->getConnName();
    size_t n = m_connectionMap.erase(conn->getConnName());
    (void)n;
    assert(n == 1);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueTask([&conn](){
        conn->connectionDestroyed();
    });
}

void TcpServer::setThreadNum(int numThreads) {
    assert(0 <= numThreads);
    m_loopThreadPool->setThreadNum(numThreads);
}


} //namespace ynet
