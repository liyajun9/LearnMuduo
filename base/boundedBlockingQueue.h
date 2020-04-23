//
// Created by ralph on 4/23/20.
//

#ifndef LEARNMUDUO_BOUNDEDBLOCKINGQUEUE_H
#define LEARNMUDUO_BOUNDEDBLOCKINGQUEUE_H


#include <mutex>
#include <condition_variable>
#include <deque>
#include "../pch.h"

namespace ybase {

template <typename T>
class BoundedBlockingQueue {
NonCopyable(BoundedBlockingQueue)

public:
    explicit BoundedBlockingQueue(size_t maxSize)
    : m_maxSize(maxSize){}

    void put(const T& x){
        std::unique_lock<std::mutex> lock(m_mtx);
        assert(m_queue.size() <= m_maxSize);
        while(m_queue.size() == m_maxSize){
            m_notFullCond.wait(lock);
        }
        assert(m_queue.size() < m_maxSize);
        m_queue.push_back(x);
        m_notEmptyCond.notify_one();
    }

    void put(T&& x){
        std::unique_lock<std::mutex> lock(m_mtx);
        assert(m_queue.size() <= m_maxSize);
        while(m_queue.size() == m_maxSize){
            m_notFullCond.wait(lock);
        }
        assert(m_queue.size() < m_maxSize);
        m_queue.push_back(std::move(x));
    }

    T take(){
        std::unique_lock<std::mutex> lock(m_mtx);
        while(m_queue.empty()){
            m_notEmptyCond.wait();
        }
        T front(m_queue.front());
        m_queue.pop_front();
        m_notFullCond.notify_one();
    }

    size_t size() const{
        std::unique_lock<std::mutex> lock(m_mtx);
        return m_queue.size();
    }

private:
    mutable std::mutex m_mtx;
    std::condition_variable m_notEmptyCond;
    std::condition_variable m_notFullCond;

    const size_t m_maxSize;
    std::deque<T> m_queue;
};

} //namespace ybase

#endif //LEARNMUDUO_BOUNDEDBLOCKINGQUEUE_H
