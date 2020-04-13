//
// Created by ralph on 4/13/20.
//

#ifndef LINUXSERVER_SOCKETUTILS_H
#define LINUXSERVER_SOCKETUTILS_H

namespace ynet {

class SocketUtils {
    NonCopyable(SocketUtils)
public:
    static ssize_t read(int sockfd, void* buf, size_t count);
    static ssize_t write(int sockfd, const void*buf, size_t count);

};

} //namespace ynet

#endif //LINUXSERVER_SOCKETUTILS_H
