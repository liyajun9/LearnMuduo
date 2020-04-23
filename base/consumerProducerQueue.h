//
// Created by ralph on 4/23/20.
//

#ifndef LEARNMUDUO_CONSUMERPRODUCERQUEUE_H
#define LEARNMUDUO_CONSUMERPRODUCERQUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include "../pch.h"

namespace ybase {

template <typename T>
class ConsumerProducerQueue {
NonCopyable(ConsumerProducerQueue)

public:
    explicit ConsumerProducerQueue(size_t maxSize)
    : m_maxSize(maxSize){}

    void add(T request){
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cond.wait(lock, [this](){ return !isFull(); });
        m_queue.push(request);
        lock.unlock();
        m_cond.notify_all();
    }

    void consume(T& request){
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cond.wait(lock, [this](){ return !isEmpty(); });
        request = m_queue.front();
        m_queue.pop();
        lock.unlock();
        m_cond.notify_all();
    }

    bool isFull() const{
        return m_queue.size() >= m_maxSize;
    }

    bool isEmpty() const{
        return m_queue.size() == 0;
    }

    int length() const{
    return m_queue.size();
    }

    void clear(){
        std::unique_lock<std::mutex> lock(m_mtx);
        while(!isEmpty())
            m_queue.pop();
        lock.unlock();
        m_cond.notify_all();
    }

private:
    mutable std::mutex m_mtx;
    std::condition_variable m_cond;

    std::queue<T> m_queue;
    const size_t m_maxSize;
};

} //namespace ybase
#endif //LEARNMUDUO_CONSUMERPRODUCERQUEUE_H
