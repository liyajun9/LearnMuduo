//
// Created by ralph on 4/22/20.
//

#include "protobufCodec.h"
#include "../../../net/buffer.h"
#include "../../../net/tcpConnection.h"
#include "../../../base/logging.h"
#include <zlib.h>

namespace yprotobuf {

int32_t asInt32(const char* buf)
{
    int32_t be32 = 0;
    ::memcpy(&be32, buf, sizeof(be32));
    return ynet::networkToHost32(be32);
}

ProtobufCodec::ProtobufCodec(MessageCallback messageCb)
: m_messageCb(std::move(messageCb))
, m_errorCb(defaultErrorCallback){

}

ProtobufCodec::ProtobufCodec(MessageCallback messageCb, ErrorCallback errorCb)
: m_messageCb(std::move(messageCb))
, m_errorCb(std::move(errorCb)){

}

void ProtobufCodec::onMessage(const ynet::TcpConnectionPtr &conn, ynet::Buffer *buf, ybase::Timestamp recvTime) {
    while(buf->readableBytes() >= (s_minMessageLen + s_headerLen)){ //minimum protobuf format: no content, typeNameLen=2
        const int32_t len = buf->peekInt32();
        if(len < s_minMessageLen || len > s_maxMessageLen){     //error message, break
            m_errorCb(conn, buf, recvTime, InvalidLength);
            break;
        }else if(buf->readableBytes() < (len + s_headerLen)){   //incomplete message, just leave it
            break;
        }else{
            ErrorCode errorCode = NoError;
            MessagePtr message = parse(buf->getReadPos() + s_headerLen, len, &errorCode);
            if(errorCode == NoError && message){
                m_messageCb(conn, message, recvTime);
                buf->retrieve(s_headerLen + len);
            }else{
                m_errorCb(conn, buf, recvTime, errorCode);
                break;
            }
        }
    }
}

void ProtobufCodec::send(const ynet::TcpConnectionPtr &conn, const google::protobuf::Message &message) {
    ynet::Buffer buf;
    fillEmptyBuffer(&buf, message);
    conn->send_mt(&buf);
}

MessagePtr ProtobufCodec::parse(const char *buf, int len, ProtobufCodec::ErrorCode *errorCode) {
    MessagePtr message;

    //check sum
    int32_t expectedCheckSum = asInt32(buf + len - s_headerLen);
    int32_t checkSum = ::adler32(1, reinterpret_cast<const Bytef*>(buf), static_cast<int>(len - s_headerLen));
    if(checkSum != expectedCheckSum){
        *errorCode = CheckSumError;
    }else{
        int32_t nameLen = asInt32(buf);
        if(nameLen < 2 || (nameLen > len - 2 * s_headerLen)){
            *errorCode = InvalidNameLen;
        }else{
            std::string typeName(buf + s_headerLen, buf + s_headerLen + nameLen - 1);
            message.reset(createMessage(typeName));
            if(!message){
                *errorCode = UnknownMessageType;
            }else{
                //parse buffer
                const char* data = buf + s_headerLen + nameLen;
                int32_t dataLen = len - nameLen - 2 * s_headerLen;
                if(!message->ParseFromArray(data, dataLen)){
                    *errorCode = ParseError;
                }else{
                    *errorCode = NoError;
                }
            }
        }
    }

    return message;
}

google::protobuf::Message *ProtobufCodec::createMessage(const std::string &typeName) {
    google::protobuf::Message* message = nullptr;
    const google::protobuf::Descriptor* descriptor =
            google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if(descriptor){
        const google::protobuf::Message* prototype =
                google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if(prototype){
            message = prototype->New();
        }
    }
    return message;
}

void ProtobufCodec::fillEmptyBuffer(ynet::Buffer *buf, const google::protobuf::Message &message) {
    assert(buf->readableBytes() == 0);

    const std::string& typeName = message.GetTypeName();
    int32_t nameLen = typeName.size() + 1;
    buf->appendInt32(nameLen);                  //int32_t nameLen
    buf->append(typeName.c_str(), nameLen);     //char    typeName[nameLen]

    int byte_size = message.ByteSizeLong();
    buf->ensureSpace(byte_size);

    uint8_t* start = reinterpret_cast<uint8_t*>(buf->getWritePos());
    uint8_t* end = message.SerializeWithCachedSizesToArray(start);
//    if(end -start != byte_size){
//        ByteSizeConsistencyError(byte_size, message.ByteSizeLong(), end-start);//used to produce an error message
//    }
    buf->hasWritten(byte_size);

    //check sum
    int32_t checksum = static_cast<int32_t>(::adler32(1, reinterpret_cast<const Bytef*>(buf->getReadPos()),
            static_cast<int>(buf->readableBytes())));
    buf->appendInt32(checksum);

    assert(buf->readableBytes() == sizeof(nameLen) + nameLen + byte_size + sizeof(checksum));
    int32_t len = ynet::hostToNetwork32(buf->readableBytes());
}

const string kNoErrorStr = "NoError";
const string kInvalidLengthStr = "InvalidLength";
const string kCheckSumErrorStr = "CheckSumError";
const string kInvalidNameLenStr = "InvalidNameLen";
const string kUnknownMessageTypeStr = "UnknownMessageType";
const string kParseErrorStr = "ParseError";
const string kUnknownErrorStr = "UnknownError";

const std::string &ProtobufCodec::errorCodeToString(ProtobufCodec::ErrorCode errorCode) {
    switch (errorCode)
    {
        case NoError:
            return kNoErrorStr;
        case InvalidLength:
            return kInvalidLengthStr;
        case CheckSumError:
            return kCheckSumErrorStr;
        case InvalidNameLen:
            return kInvalidNameLenStr;
        case UnknownMessageType:
            return kUnknownMessageTypeStr;
        case ParseError:
            return kParseErrorStr;
        default:
            return kUnknownErrorStr;
    }
}

void ProtobufCodec::defaultErrorCallback(const ynet::TcpConnectionPtr &conn, ynet::Buffer *buf,
                                         ybase::Timestamp timestamp, int errCode) {
    LOG_ERROR << "ProtobufCodec::defaultErrorCallback - " << errorCodeToString((ErrorCode)errCode);
    if(conn && conn->isConnected()){ //default to shutdown connection
        conn->shutdown_mt();
    }
}


} //namespace yprotobuf