//
// Created by ralph on 4/11/20.
//

#ifndef LINUXSERVER_CHANNEL_H
#define LINUXSERVER_CHANNEL_H

#include <functional>
#include <sys/poll.h>

namespace ynet {
    class EventLoop;
    class Poller;

/* Channel: responsible for fd events management and event dispatching
 * Lifecycle:
 */
class Channel {
public:
    using ReadCallback = std::function<void()>;
    using WriteCallback = std::function<void()>;
    using ErrorCallback = std::function<void()>;

public:
    Channel(EventLoop* loop, int fd);

    void handleEvents();

    void updateChannel();

    void enableRead() { m_events |= ReadEvent; updateChannel(); }
    void enableWrite() { m_events |= WriteEvent; updateChannel(); }
    void disableRead() { m_events &= ~ReadEvent;  updateChannel(); }
    void disableWrite() { m_events &= ~WriteEvent; updateChannel(); }
    void disableAll() { m_events = NoneEvent; updateChannel(); }

    void setReadCallback(const ReadCallback& readCb) { m_readCb = readCb; }
    void setWriteCallback(const WriteCallback& writeCb) { m_writeCb = writeCb; }
    void setErrorCallback(const ErrorCallback& errorCb) { m_errorCb = errorCb; }
    void setCurrEvents(int currEvents) { m_currEvents = currEvents; }
    void setIndex(int index) { m_index = index; }

    bool isNoneEvent() const { return m_events == NoneEvent; }

    int getFd() const { return m_fd; }
    int getEvents() const { return m_events; }
    int getIndex() const { return m_index; }
    EventLoop* getOwnerLoop() const { return m_ownerLoop; }

private:
    static constexpr int NoneEvent = 0;
    static constexpr int ReadEvent = POLLIN | POLLPRI;
    static constexpr int WriteEvent = POLLOUT;

    int m_fd;
    int m_events;
    int m_currEvents;
    int m_index;    //index in Poller::fdlist

    ReadCallback m_readCb;
    WriteCallback m_writeCb;
    ErrorCallback m_errorCb;

    EventLoop* m_ownerLoop;
};

} //namespace ynet
#endif //LINUXSERVER_CHANNEL_H
