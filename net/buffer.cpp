//
// Created by ralph on 3/28/20.
//

#include "buffer.h"
#include "socketUtils.h"

namespace ynet {

ssize_t Buffer::readFromFd(int fd, int *savedErrorno) {
    char extraBuf[65536];
    struct iovec vec[2];
    
    //1st part: our buffer
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + m_writePos;
    vec[0].iov_len = writable;
    
    //2nd part(spare part): etrabuf
    vec[1].iov_base = extraBuf;
    vec[1].iov_len = sizeof(extraBuf);
    
    //use extraBuf in case out buffer size is with <64kB left
    const int iovcnt = (writable < sizeof(extraBuf)) ? 2 : 1;
    const ssize_t n = SocketUtils::readv(fd, vec, iovcnt);
    if(n < 0)
        *savedErrorno = errno;
    else if( n <= writable)
        m_writePos += n;
    else{
        m_writePos = m_vec.size();
        append(extraBuf, n - writable); //append extraBuf to vector
    }
    return n;
}

} //namespace ynet