//
// Created by ralph on 4/23/20.
//

#ifndef LEARNMUDUO_BLOCKINGQUEUE_H
#define LEARNMUDUO_BLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>
#include "../pch.h"

namespace ybase {

template<typename T>
class BlockingQueue {
    NonCopyable(BlockingQueue)
public:
    BlockingQueue() = default;

    void put(const T& x){
        std::unique_lock<std::mutex> lock(m_mtx);
        m_queue.push_back(x);
        m_notEmptyCond.notify_one();
    }

    void put(T&& x){
        std::unique_lock<std::mutex> lock(m_mtx);
        m_queue.push_back(std::move(x));
        m_notEmptyCond.notify_one();
    }

    T take(){
        std::unique_lock<std::mutex> lock(m_mtx);
        while(m_queue.empty())
            m_notEmptyCond.wait(lock);
        T front(std::move(m_queue.front()));
        m_queue.pop_front();
        return front;
    }

    size_t size() const{
        std::unique_lock<std::mutex> lock(m_mtx);
        return m_queue.size();
    }

private:
    mutable std::mutex m_mtx;
    std::condition_variable m_notEmptyCond;

    std::deque<T> m_queue;
};

} //namespace ybase

#endif //LEARNMUDUO_BLOCKINGQUEUE_H
