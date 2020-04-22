//
// Created by ralph on 3/28/20.
//

#ifndef LEARNMUDUO_BUFFER_H
#define LEARNMUDUO_BUFFER_H

#include <vector>
#include <assert.h>
#include <cstring>
#include "endian.h"

namespace ynet{

class Buffer {
public:
    static constexpr size_t initialSize = 1024;

    explicit Buffer(size_t readPos = 0, size_t writePos = 0):
    m_readPos(readPos)
    ,m_writePos(writePos)
    ,m_vec(initialSize){}

    size_t readableBytes() const{
        return m_writePos - m_readPos;
    }

    const char* getReadPos() const{
        return begin() + m_readPos;
    }

    size_t writableBytes() const{
        return m_vec.size() - m_writePos;
    }

    char* getWritePos() const {
        return const_cast<char *>(begin() + m_writePos);
    }

    /* write to Buffer
     *
     */
    void appendInt32(int32_t x){
        int32_t be32 = hostToNetwork32(x);
        append(&be32, sizeof(be32));
    }

    void append(const void* data, int len){
        append(reinterpret_cast<const char*>(data), len);
    }

    void append(const char* data, int len){
        ensureSpace(len);
        std::copy(data, data + len, m_vec.begin() + m_writePos);
        assert(len <= writableBytes());
        m_writePos += len;
        assert(m_writePos < m_vec.size());
    }

    ssize_t readFromFd(int fd, int* savedError);

    void hasWritten(size_t len){
        assert(len <= writableBytes());
        m_writePos += len;
    }

    /* read from Buffer
     *
     */
    void retrieve(size_t len){
        assert(len <= readableBytes());
        if(len < readableBytes()){
            m_readPos += len;
        }else{
            retrieveAll();
        }
    }

    void retrieveAll(){
        m_readPos = 0;
        m_writePos = 0;
    }

    /* peek from Buffer
     *
     */
    int32_t peekInt32() const {
        assert(readableBytes() >= sizeof(int32_t));
        int32_t be32 = 0;
        ::memcpy(&be32, getReadPos(), sizeof(be32));
        return networkToHost32(be32);
    }

    void ensureSpace(size_t len){
        if(len <= writableBytes())
            return;
        else
            makeSpace(len);
        assert(len <= writableBytes());
    }

private:
    char* begin(){
        return &m_vec[0];
    }

    const char* begin() const{
        return &m_vec[0];
    }

    void makeSpace(size_t len){
        if(len <= (m_readPos + writableBytes())){ //copy
            std::copy(m_vec.begin() + m_readPos, m_vec.begin() + m_writePos, m_vec.begin());
            m_writePos -= m_readPos;
            m_readPos = 0;
        }else{
            m_vec.resize(m_vec.size() + len);
        }
    }

private:
    std::vector<char> m_vec;
    size_t m_readPos;
    size_t m_writePos;
};

}



#endif //LEARNMUDUO_BUFFER_H
