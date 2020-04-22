//
// Created by ralph on 4/22/20.
//

#include "dispatcher_lite.h"

namespace yprotobuf {


Dispatcher_lite::Dispatcher_lite(MessageCallback defaultCb)
: m_defaultCallback(std::move(defaultCb)){

}

void Dispatcher_lite::onProtobufMessage(const ynet::TcpConnectionPtr &conn, const MessagePtr &message, ybase::Timestamp recvTime) {
    auto it = m_callbackMap.find(message->GetDescriptor());
    if(it != m_callbackMap.end()){
        it->second(conn, message, recvTime);
    }else{
        m_defaultCallback(conn, message, recvTime);
    }
}

void Dispatcher_lite::registerMessageCallback(const google::protobuf::Descriptor *desc, MessageCallback messageCb) {
    m_callbackMap[desc] = std::move(messageCb);
}

} //namespace yprotobuf