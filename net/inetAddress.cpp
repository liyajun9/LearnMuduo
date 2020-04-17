//
// Created by ralph on 4/17/20.
//

#include <netdb.h>
#include "inetAddress.h"
#include "endian.h"
#include "socketUtils.h"
#include "../base/logging.h"

namespace ynet{


InetAddress::InetAddress(uint16_t port, bool loopbackonly, bool ipv6) {
    if(ipv6){
        ybase::memZero(&m_addr6, sizeof(m_addr6));
        m_addr6.sin6_family = AF_INET6;
        in6_addr ip = loopbackonly ? in6addr_loopback : in6addr_any;
        m_addr6.sin6_addr = ip;
        m_addr6.sin6_port = hostToNetwork16(port);
    }else{
        ybase::memZero(&m_addr, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        in_addr_t ip = loopbackonly ? INADDR_LOOPBACK : INADDR_ANY;
        m_addr.sin_addr.s_addr = hostToNetwork32(ip);
        m_addr.sin_port = hostToNetwork16(port);
    }
}

InetAddress::InetAddress(ybase::StringArg ip, uint16_t port, bool ipv6) {
    if(ipv6){
        ybase::memZero(&m_addr6, sizeof(m_addr6));
        SocketUtils::fromIpPort(ip.c_str(), port, &m_addr6);
    }else{
        ybase::memZero(&m_addr, sizeof(m_addr));
        SocketUtils::fromIpPort(ip.c_str(), port, &m_addr);
    }
}

InetAddress::InetAddress(const struct sockaddr_in &addr)
    : m_addr(addr){

}

InetAddress::InetAddress(const struct sockaddr_in6 &addr)
    : m_addr6(addr){

}

std::string InetAddress::toIp() const {
    char buf[64] = "";
    SocketUtils::toIp(buf, sizeof(buf), getSockAddr());
}

std::string InetAddress::toIpPort() const {
    char buf[64] = "";
    SocketUtils::toIpPort(buf, sizeof buf, getSockAddr());
    return buf;
}

uint16_t InetAddress::toPort() const {
    return networkToHost16(portNetEndian());;
}

uint32_t InetAddress::ipNetEndian() const {
    assert(family() == AF_INET);
    return m_addr.sin_addr.s_addr;
}

static __thread char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(ybase::StringArg hostname, InetAddress *result) {
    assert(result != NULL);
    struct hostent hent;
    struct hostent* he = NULL;
    int herrno = 0;
    ybase::memZero(&hent, sizeof(hent));

    int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);
    if (ret == 0 && he != NULL)
    {
        assert(he->h_addrtype == AF_INET && he->h_length == sizeof(uint32_t));
        result->m_addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    }
    else
    {
        if (ret)
        {
            LOG_SYSERR << "InetAddress::resolve";
        }
        return false;
    }
}

void InetAddress::setCopeId(uint32_t scope_id) {
    if (family() == AF_INET6)
    {
        m_addr6.sin6_scope_id = scope_id;
    }
}
} //namespace ynet