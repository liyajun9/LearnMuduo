//
// Created by ralph on 4/13/20.
//

#ifndef LEARNMUDUO_SOCKETUTILS_H
#define LEARNMUDUO_SOCKETUTILS_H

#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/un.h>

namespace ynet {

class SocketUtils {
    NonCopyable(SocketUtils)
public:
    static ssize_t read(int sockfd, void* buf, size_t count);
    static ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);
    static ssize_t write(int sockfd, const void*buf, size_t count);
    static void close(int sockfd);
    static void shutdownWrite(int sockfd);

    static int createNonblockingOrDie(sa_family_t family);
    static int connect(int sockfd, const struct sockaddr* addr);
    static void bindOrDie(int sockfd, const struct sockaddr* addr);
    static void listenOrDie(int sockfd);
    static int accept(int sockfd, struct sockaddr_in6* addr);

    static void toIpPort(char* buf, size_t size, const struct sockaddr* addr);
    static void toIp(char* buf, size_t size, const struct sockaddr* addr);
    static void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in* addr);
    static void fromIpPort(const char* ip, uint16_t port, struct sockaddr_in6* addr);

    static int getSocketError(int sockfd);
    static struct sockaddr_in6 getLocalAddr(int sockfd);
    static struct sockaddr_in6 getPeerAddr(int sockfd);

    static bool isSelfConnect(int sockfd);

    static const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
    static const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
    static struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);
    static const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
    static const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);
};

} //namespace ynet

#endif //LEARNMUDUO_SOCKETUTILS_H
