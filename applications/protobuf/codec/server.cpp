//
// Created by ralph on 4/22/20.
//

#include "server.h"
#include "../../../base/logging.h"
#include "../../../net/tcpConnection.h"

namespace yprotobuf {


Server::Server(ynet::EventLoop *loop, const ynet::InetAddress &listenAddr)
: m_server(loop, listenAddr, "yprotobuf::server")
, m_dispatcher([this](const ynet::TcpConnectionPtr &conn, const std::shared_ptr<google::protobuf::Message> &message, ybase::Timestamp timestamp){
    this->onUnknownMessage(conn, message, timestamp);
})
, m_codec([this](const ynet::TcpConnectionPtr &conn, const std::shared_ptr<google::protobuf::Message> &message, ybase::Timestamp timestamp){
    this->m_dispatcher.onProtobufMessage(conn, message, timestamp);
}){
    // register callbacks for different type of messages
     m_dispatcher.registerMessageCallback(ynet::QueryResults::GetDescriptor(), [this](const ynet::TcpConnectionPtr& conn, const MessagePtr& message, ybase::Timestamp timestamp){
         this->onQueryResults(conn, message, timestamp);
     });

    //set connection up or down callback
    m_server.setConnectionChangeCallback([this](const ynet::TcpConnectionPtr &conn){
        this->onConnectionChange(conn);
    });

    //set message callback to avoid manually process binary buffer
    m_server.setMessageCallback([this](const ynet::TcpConnectionPtr& conn, ynet::Buffer* buf, ybase::Timestamp recvTime){
        this->m_codec.onMessage(conn, buf, recvTime);
    });
}

void Server::start() {
    m_server.start();
}

void Server::onConnectionChange(const ynet::TcpConnectionPtr &conn) {
    LOG_INFO << conn->getLocalAddr().toIpPort() << " -> "
             << conn->getPeerAddr().toIpPort() << " is "
             << (conn->isConnected() ? "UP" : "DOWN");
}

void Server::onUnknownMessage(const ynet::TcpConnectionPtr &conn,
                              const std::shared_ptr<google::protobuf::Message> &message,
                              ybase::Timestamp timestamp) {
    LOG_INFO << "onUnknownMessage: " << message->GetTypeName();
    conn->shutdown_mt();
}

void Server::onQueryResults(const ynet::TcpConnectionPtr &conn, const MessagePtr &message, ybase::Timestamp timestamp) {
    LOG_INFO << "onQueryResults:\n" << message->GetTypeName() << message->DebugString();
    ynet::QueryResults* queryResult = dynamic_cast<ynet::QueryResults*>(message.get());
    if(queryResult){
        ynet::AnswerResults answer;
        answer.set_page_number(99);
        answer.set_datavalue(1, "this the first answer");
        m_codec.send(conn, answer);
    }

}

} //namespace yprotobuf