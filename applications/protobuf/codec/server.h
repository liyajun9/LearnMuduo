//
// Created by ralph on 4/22/20.
//

#ifndef LEARNMUDUO_SERVER_H
#define LEARNMUDUO_SERVER_H

#include <google/protobuf/message.h>
#include "../../../net/tcpServer.h"
#include "protobufCodec.h"
#include "dispatcher_lite.h"
#include "../../../proto/request.pb.h"

namespace yprotobuf {

class Server {
NonCopyable(Server)
public:
    Server(ynet::EventLoop* loop, const ynet::InetAddress& listenAddr);
    ~Server() = default;

    void start();

private:
    void onConnectionChange(const ynet::TcpConnectionPtr& conn);
    void onUnknownMessage(const ynet::TcpConnectionPtr& conn, const std::shared_ptr<google::protobuf::Message>& message, ybase::Timestamp timestamp);

    void onQueryResults(const ynet::TcpConnectionPtr& conn, const MessagePtr& message, ybase::Timestamp timestamp);

private:
    ynet::TcpServer m_server;
    Dispatcher_lite m_dispatcher;
    ProtobufCodec m_codec;
};

} //namespace yprotobuf

#endif //LEARNMUDUO_SERVER_H
