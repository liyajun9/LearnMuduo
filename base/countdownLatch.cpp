//
// Created by ralph on 4/3/20.
//

#include "countdownLatch.h"

namespace ybase{
CountdownLatch::CountdownLatch(int count)
:m_count(count)
{

}

void CountdownLatch::wait() {
    std::unique_lock<std::mutex> lock(m_mtx);
    while(m_count > 0){
        m_cond.wait(lock);
    }
}

void CountdownLatch::countDown() {
    std::unique_lock<std::mutex> lock(m_mtx);
    if(--m_count == 0){
        m_cond.notify_all();
    }
}

int CountdownLatch::getCount() const {
    std::unique_lock<std::mutex> lock(m_mtx);
    return m_count;
}

}//namespace ynet

