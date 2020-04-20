//
// Created by ralph on 4/17/20.
//

#include <memory>
#include <functional>
#include <set>
#include "../base/timestamp.h"

#ifndef LINUXSERVER_ALIAS_H
#define LINUXSERVER_ALIAS_H

#endif //LINUXSERVER_ALIAS_H

namespace ynet{
    class TcpConnection;
    class Buffer;
    class InetAddress;
    class EventLoop;
    class Timer;

    //EventLoopThread
    using LoopThreadInitCallback = std::function<void(EventLoop*)>;

    //EventLoop
    using AsyncTask = std::function<void()>;

    //Poller
    using Pollfd = struct pollfd;

    //Channerl
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(ybase::Timestamp timestamp)>;

    //Timer, TimerQueue
    using TimerCallback = std::function<void()>;

    using TimerEntry = std::pair<ybase::Timestamp, Timer*>;
    using TimerList = std::set<TimerEntry>;

    using TimerIdEntry = std::pair<Timer*, int64_t>; //raw pointers of Timer, so must operator together with TimerList.
    using TimerIdList = std::set<TimerIdEntry>;


    //TcpServer
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

    //Acceptor
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;

    //TcpConnection
    using ConnectionChangeCallback = std::function<void(const TcpConnectionPtr& conn)>;
    using CloseCallback = std::function<void(const TcpConnectionPtr& conn)>;
    using MessageCallback = std::function<void(const TcpConnectionPtr& conn, Buffer* buf, ybase::Timestamp recvTime)>;
    using WriteCompleteCallback = std::function<void(const TcpConnectionPtr& conn)>;
    using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr& conn, size_t n)>;
} //namespace ynet