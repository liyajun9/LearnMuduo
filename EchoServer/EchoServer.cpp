//
// Created by ralph on 3/25/20.
//

#include "EchoServer.h"

EchoServer::EchoServer(muduo::net::EventLoop *loop, const muduo::net::InetAddress &listenAddr)
:m_loop(loop)
,m_server(loop, listenAddr, "EchoServer")
{
//    m_server.setConnectionCallback(std::bind(&EchoServer::OnConnection, this, std::placeholders::_1));
    m_server.setConnectionCallback([this](const muduo::net::TcpConnectionPtr& conn){
        this->OnConnection(conn);
    });
    m_server.setMessageCallback([this](const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time){
        this->OnMessage(conn, buf, time);
    });
}

void EchoServer::start() {
    m_server.start();
}

void EchoServer::OnConnection(const muduo::net::TcpConnectionPtr &conn) {
    LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << "->"
    << conn->localAddress().toIpPort() << " is "
    << (conn->connected() ? "UP" : "DOWN");
}

void EchoServer::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time) {
    muduo::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, " << "data received at " << time.toString();
    conn->send(msg);
}
