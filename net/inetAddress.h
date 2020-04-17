//
// Created by ralph on 4/17/20.
//

#ifndef LINUXSERVER_INETADDRESS_H
#define LINUXSERVER_INETADDRESS_H

#include <netinet/in.h>
#include "../base/stringPiece.h"
#include "socketUtils.h"

namespace ynet {

class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackonly = false, bool ipv6 = false);
    InetAddress(ybase::StringArg ip, uint16_t port, bool ipv6 = false);
    explicit InetAddress(const struct sockaddr_in& addr);
    explicit InetAddress(const struct sockaddr_in6& addr);

    sa_family_t family() const { return m_addr.sin_family; }
    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const struct sockaddr* getSockAddr() const { return SocketUtils::sockaddr_cast(&m_addr6); }
    void setSockAddInet6(const struct sockaddr_in6& addr6) { m_addr6 = addr6; }

    uint32_t ipNetEndian() const;
    uint16_t portNetEndian() const { return m_addr.sin_port; }

    static bool resolve(ybase::StringArg hostname, InetAddress* result);

    void setCopeId(uint32_t scope_id);

private:
    union{
        struct sockaddr_in m_addr;
        struct sockaddr_in6 m_addr6;
    };
};

} //namespace ynet

#endif //LINUXSERVER_INETADDRESS_H
