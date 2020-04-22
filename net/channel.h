//
// Created by ralph on 4/11/20.
//

#ifndef LEARNMUDUO_CHANNEL_H
#define LEARNMUDUO_CHANNEL_H

#include <functional>
#include <sys/poll.h>
#include "alias.h"

namespace ynet {
    class EventLoop;
    class Poller;

/* Channel: responsible for fd events management and event dispatching
 * Lifecycle:
 */
class Channel {

public:
    Channel(EventLoop* loop, int fd);

    void handleEvents(ybase::Timestamp timestamp);

    //any operation on events will invoke update to add channel to poll_fd
    void enableRead() { m_events |= ReadEvent;        update(); }
    void enableWrite() { m_events |= WriteEvent;        update(); }
    void disableRead() { m_events &= ~ReadEvent;        update(); }
    void disableWrite() { m_events &= ~WriteEvent;        update(); }
    void disableAll() { m_events = NoneEvent;        update(); }
    bool isWriting() const { return m_events & WriteEvent; }
    bool isReading() const { return m_events & ReadEvent; }

    void remove();

    void setReadCallback(ReadEventCallback readCb) { m_readCb = std::move(readCb); }
    void setWriteCallback(EventCallback writeCb) { m_writeCb = std::move(writeCb); }
    void setCloseCallback(EventCallback closeCb) { m_closeCb = std::move(closeCb); }
    void setErrorCallback(EventCallback errorCb) { m_errorCb = std::move(errorCb); }

    void setCurrEvents(int currEvents) { m_currEvents = currEvents; }
    void setIndex(int index) { m_index = index; }

    bool isNoneEvent() const { return m_events == NoneEvent; }

    void tie(const std::shared_ptr<void>& ptr);

    int getFd() const { return m_fd; }
    int getEvents() const { return m_events; }
    int getIndex() const { return m_index; }
    EventLoop* getOwnerLoop() const { return m_ownerLoop; }

private:
    void update();

private:
    static constexpr int NoneEvent = 0;
    static constexpr int ReadEvent = POLLIN | POLLPRI;
    static constexpr int WriteEvent = POLLOUT;

    int m_fd;
    int m_events;
    int m_currEvents;
    int m_index;    //index in Poller::fdlist

    ReadEventCallback m_readCb;
    EventCallback m_writeCb;
    EventCallback m_closeCb;
    EventCallback m_errorCb;

    std::weak_ptr<void> m_tie; //used to extend life ot sth until handleEvents finished.(especially TcpConnection)
    bool m_tied;

    EventLoop* m_ownerLoop;
};

} //namespace ynet
#endif //LEARNMUDUO_CHANNEL_H
