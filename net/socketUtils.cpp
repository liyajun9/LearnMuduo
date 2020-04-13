//
// Created by ralph on 4/13/20.
//

#include <unistd.h>
#include "socketUtils.h"

namespace ynet{


ssize_t SocketUtils::read(int sockfd, void *buf, size_t count) {
    return ::read(sockfd, buf, count);
}

ssize_t SocketUtils::write(int sockfd, const void *buf, size_t count) {
    return ::write(sockfd, buf, count);
}



} //namespace ynet