//
// Created by ralph on 4/22/20.
//

#ifndef LEARNMUDUO_PROTOBUFCODEC_H
#define LEARNMUDUO_PROTOBUFCODEC_H

#include <google/protobuf/message.h>
#include "../../../net/alias.h"

//packet format
//struct ProtobufTransportFormat __attribute__ ((__parked))
//{
//    int32_t len;
//    int32_t nameLen;
//    char    typeName[nameLen];
//    char    protobufData[len-4-nameLen-4];
//    int32_t checkSum; //adler32(nameLen, typeName, protobufData)
//}

namespace yprotobuf {

    using MessagePtr = std::shared_ptr<google::protobuf::Message>;
    using MessageCallback = std::function<void(const ynet::TcpConnectionPtr&, const MessagePtr&, ybase::Timestamp)>;
    using ErrorCallback = std::function<void(const ynet::TcpConnectionPtr&, ynet::Buffer*, ybase::Timestamp, int)>;

class ProtobufCodec {
NonCopyable(ProtobufCodec)
public:
    enum ErrorCode{
        NoError = 0,
        InvalidLength,
        CheckSumError,
        InvalidNameLen,
        UnknownMessageType,
        ParseError,
    };

    explicit ProtobufCodec(MessageCallback messageCb);
    ProtobufCodec(MessageCallback messageCb, ErrorCallback errorCb);
    ~ProtobufCodec() = default;

    void onMessage(const ynet::TcpConnectionPtr& conn, ynet::Buffer* buf, ybase::Timestamp timestamp);//key func: handle received buffer

    void send(const ynet::TcpConnectionPtr& conn, const google::protobuf::Message& message);

public:
    static MessagePtr parse(const char* buf, int len, ErrorCode* errorCode);
    static google::protobuf::Message* createMessage(const std::string& typeName);
    static void fillEmptyBuffer(ynet::Buffer* buf, const google::protobuf::Message& message);
    static const std::string& errorCodeToString(ErrorCode errorCode);

private:
    static void defaultErrorCallback(const ynet::TcpConnectionPtr& conn, ynet::Buffer* buf, ybase::Timestamp recvTime, int errCode);

private:
    MessageCallback m_messageCb;
    ErrorCallback   m_errorCb;

    static constexpr int s_headerLen = sizeof(int32_t);
    static constexpr int s_minMessageLen = 2 * s_headerLen + 2; // nameLen + typename + checkSum
    static constexpr int s_maxMessageLen = 64 * 1024 * 1024; //64MB
};

} //namespace yprotobuf

#endif //LEARNMUDUO_PROTOBUFCODEC_H
