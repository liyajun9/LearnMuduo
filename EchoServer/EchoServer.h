//
// Created by ralph on 3/25/20.
//

#ifndef LINUXSERVER_ECHOSERVER_H
#define LINUXSERVER_ECHOSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>

class EchoServer {
public:
    EchoServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress& listenAddr);

    void start(); //calls m_server.start();

private:
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time);

    muduo::net::EventLoop* m_loop;
    muduo::net::TcpServer m_server;
};


#endif //LINUXSERVER_ECHOSERVER_H
