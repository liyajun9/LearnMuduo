//
// Created by ralph on 4/17/20.
//

#include "tcpConnection.h"
#include "eventLoop.h"
#include "socket.h"
#include "channel.h"
#include "../base/logging.h"

namespace ynet{


TcpConnection::TcpConnection(EventLoop *loop, std::string name, int sockfd, const InetAddress &localAddr,
                             const InetAddress &peerAddr)
: m_loop(loop)
, m_connName(std::move(name))
, m_state(CONNECTING)
, m_reading(true) //first thing is reading
, m_socket(std::unique_ptr<Socket>(new Socket(sockfd)))
, m_channel(std::unique_ptr<Channel>(new Channel(loop, sockfd)))
, m_localAddr(localAddr)
, m_peerAddr(peerAddr){
    m_channel->setReadCallback([this](ybase::Timestamp timestamp){
        this->handleRead(timestamp);
    });
    m_channel->setWriteCallback([this](){
        this->handleWrite();
    });
    m_channel->setCloseCallback([this](){
        this->handleClose();
    });
    m_channel->setErrorCallback([this](){
        this->handleError();
    });
    m_socket->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG << "TcpConnection::dtor[" << m_connName << "] at " << this << " fd=" << m_channel->getFd()
    << " state=" << m_state;
    assert(m_state == DISCONNECTED);
}

void TcpConnection::handleRead(ybase::Timestamp recvTime) {
    m_loop->assertInCurrentThread();
    int savedErrno = 0;
    ssize_t n = m_recvBuf.readFromFd(m_channel->getFd(), &savedErrno);
    if(n > 0)
        m_messageCb(shared_from_this(), &m_recvBuf, recvTime);
    else if(n == 0)
        handleClose();
    else{
        errno = savedErrno;
        LOG_SYSERR << "TcpConnection::handleRead";
        handleError();
    }
}

void TcpConnection::handleWrite() {
    m_loop->assertInCurrentThread();
    if(m_channel->isWriting()){ //write only when you have data to send
        ssize_t n = SocketUtils::write(m_channel->getFd(), m_sendBuf.getReadPos(), m_sendBuf.readableBytes());
        if(n > 0){
            m_sendBuf.retrieve(n);
            if(m_sendBuf.readableBytes() == 0){
                m_channel->disableWrite();
                if(m_writeCompleteCb){
                    m_loop->postTask_mt([this](){
                        this->m_writeCompleteCb(this->shared_from_this());
                    });
                }
                if(m_state == DISCONNECTING){
                    shutdownInLoop();
                }
            }
        }else{
            LOG_SYSERR << "TcpConnection::handleWrite";
        }
    }else{
        LOG_TRACE << "Connection fd = " << m_channel->getFd() << " is down, no more writing";
    }
}

void TcpConnection::handleClose() {
    m_loop->assertInCurrentThread();
    LOG_TRACE << "fd = " << m_channel->getFd() << " state = " << m_state;
    assert(m_state == CONNECTED || m_state == DISCONNECTED);

    setState(DISCONNECTED);
    m_channel->disableAll();

    TcpConnectionPtr conn = shared_from_this(); //make this' life long enough until callbacks executed
    m_connectionChangeCb(conn);
    m_closeCb(conn);
}

void TcpConnection::handleError() {
    int err = SocketUtils::getSocketError(m_channel->getFd());
    LOG_ERROR << "TcpConnection::handleError [" << m_connName << "] - SO_ERROR = " << err << " " << strerror(err);
}

void TcpConnection::connectionEstablished() {
    m_loop->assertInCurrentThread();
    assert(m_state == CONNECTING);
    setState(CONNECTED);
    m_channel->tie(shared_from_this()); //save shared_ptr<>(this) into channel
    m_channel->enableRead();

    m_connectionChangeCb(shared_from_this());
}

void TcpConnection::connectionDestroyed() {
    m_loop->assertInCurrentThread();
    if(m_state == CONNECTED){
        setState(DISCONNECTED);
        m_channel->disableAll();
        m_connectionChangeCb(shared_from_this());
    }
    m_channel->remove();
}

void TcpConnection::shutdownInLoop() {
    m_loop->assertInCurrentThread();
    if(!m_channel->isWriting()){
        m_socket->shutdownWrite();
    }
}

void TcpConnection::shutdown_mt() {
    if(m_state == CONNECTED){
        setState(CONNECTING);
        m_loop->postTask_mt([this](){
            this->shutdownInLoop();
        });
    }
}

void TcpConnection::send_mt(const std::string &message) {
    send_mt(message.data(), message.size());
}

void TcpConnection::send_mt(const void *message, int len) {
    if(m_state == CONNECTED){
        if(m_loop->isInLoopThread()){
            sendInLoop(message, len);
        }else{
            m_loop->postTask_mt([this, &message, len]{
                this->sendInLoop(message, len);
            });
        }
    }
}

void TcpConnection::send_mt(const ybase::StringPiece &message) {
    if(m_state == CONNECTED){
        if(m_loop->isInLoopThread()){
            sendInLoop(message);
        }else{
            m_loop->postTask_mt([this, &message]{
                this->sendInLoop(message);
            });
        }
    }
}

void TcpConnection::send_mt(Buffer *message) {
    if(m_state == CONNECTED){
        if(m_loop->isInLoopThread()){
            sendInLoop(message->getReadPos(), message->readableBytes());
            message->retrieveAll();
        }else{
            m_loop->postTask_mt([this, message](){
                this->send_mt(message);
            });
        }
    }
}

void TcpConnection::sendInLoop(const std::string& message) {
    sendInLoop(message.data(), message.size());
}

void TcpConnection::sendInLoop(const ybase::StringPiece &message) {
    sendInLoop(message.data(), message.size());
}

void TcpConnection::sendInLoop(const void *message, size_t len) {
    m_loop->assertInCurrentThread();
    if(m_state == DISCONNECTED){
        LOG_WARN << "disconnected, give up writing";
        return;
    }

    size_t remain = len;
    ssize_t sent = 0;
    bool faultError = false;

    //nothing in sendBuf, send directly
    if(!m_channel->isWriting() && m_sendBuf.readableBytes() == 0){
        sent = ::write(m_channel->getFd(), message, len);
        if(sent >= 0){
            remain = len - sent;
            if(remain == 0){ //sent all
                if(m_writeCompleteCb)
                    m_loop->postTask_mt([this](){
                        this->m_writeCompleteCb(this->shared_from_this());
                    });
            }
        }else{
            if(errno != EWOULDBLOCK)
                LOG_SYSERR << "TcpConnection::sendInLoop";
            if(errno == EPIPE || errno == ECONNRESET)
                faultError = true;
        }
    }

    assert(sent >= 0);
    //something in sendBuf or current write only write a part of message: append to sendBuf
    //enable write
    if(!faultError &&  remain > 0){ //in case of faultError, forbid to send more

        //highWaterMark: to avoid buffer full
        size_t oldLen = m_sendBuf.readableBytes();
        if(oldLen + remain >= m_highWaterMark && oldLen < m_highWaterMark){
            if(m_highWaterMark)
                m_loop->postTask_mt([this, oldLen, remain](){
                    this->m_highWaterMarkCb(this->shared_from_this(), oldLen + remain);
                });
        }

        m_sendBuf.append(static_cast<const char*>(message) + sent, remain);
        if(!m_channel->isWriting())
            m_channel->enableWrite();
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    m_socket->setTcpNoDelay(on);
}

void TcpConnection::startRead() {
    m_loop->postTask_mt([this](){
        this->startReadInLoop();
    });
}

void TcpConnection::stopRead() {
    m_loop->postTask_mt([this](){
        this->stopReadInLoop();
    });
}

void TcpConnection::startReadInLoop() {
    m_loop->assertInCurrentThread();
    if(!m_reading || !m_channel->isReading()){
        m_channel->enableRead();
        m_reading = true;
    }
}

void TcpConnection::stopReadInLoop() {
    m_loop->assertInCurrentThread();
    if(m_reading || m_channel->isReading()){
        m_channel->disableRead();
        m_reading = false;
    }
}

void TcpConnection::forceClose() {
    if(m_state == CONNECTED || m_state == DISCONNECTING){
        setState(DISCONNECTING);
        m_loop->queueTask([this](){
            this->forceCloseInLoop();
        });
    }
}

void TcpConnection::forceCloseWithDelay(double seconds) {
    if(m_state == CONNECTED || m_state == DISCONNECTING){
        TimerCallback cb = [this](){
            this->forceCloseInLoop();
        };
        m_loop->runAfter(seconds, cb);
    }
}

void TcpConnection::forceCloseInLoop() {
    m_loop->assertInCurrentThread();
    if(m_state == CONNECTED || m_state == DISCONNECTING)
        handleClose();
}


} //namespace ynet