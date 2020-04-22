//
// Created by ralph on 4/22/20.
//

#ifndef LEARNMUDUO_DISPATCHER_LITE_H
#define LEARNMUDUO_DISPATCHER_LITE_H

#include <google/protobuf/descriptor.h>
#include "protobufCodec.h"

namespace yprotobuf {
using CallbackMap = std::map<const google::protobuf::Descriptor*, MessageCallback>; //protobuf Descriptor : messageCallback

class Dispatcher_lite {
NonCopyable(Dispatcher_lite)

public:
    explicit Dispatcher_lite(MessageCallback defaultCb);

    void onProtobufMessage(const ynet::TcpConnectionPtr& conn, const MessagePtr& message, ybase::Timestamp recvTime);

    void registerMessageCallback(const google::protobuf::Descriptor* desc, MessageCallback messageCb);

private:
    CallbackMap m_callbackMap;
    MessageCallback m_defaultCallback;
};

} //namespace yprotobuf

#endif //LEARNMUDUO_DISPATCHER_LITE_H
