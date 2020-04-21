//
// Created by ralph on 4/21/20.
//

#ifndef LINUXSERVER_PROTOBUFCODECLITE_H
#define LINUXSERVER_PROTOBUFCODECLITE_H

namespace ynet {

class ProtobufCodecLite {
NonCopyable(ProtobufCodecLite)

public:
    ProtobufCodecLite();
    ~ProtobufCodecLite();

public:
    constexpr static int s_headerLen = sizeof(int32_t);
    constexpr static int s_checkSumLen = sizeof(int32_t);
};

} //namespace ynet
#endif //LINUXSERVER_PROTOBUFCODECLITE_H
