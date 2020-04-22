//
// Created by ralph on 4/3/20.
//

#ifndef LEARNMUDUO_COUNTDOWNLATCH_H
#define LEARNMUDUO_COUNTDOWNLATCH_H

#include <mutex>
#include <condition_variable>

namespace ybase {

class CountdownLatch {
    NonCopyable(CountdownLatch)

public:
    explicit CountdownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    mutable std::mutex m_mtx;
    std::condition_variable m_cond;
    int m_count;
};

}
#endif //LEARNMUDUO_COUNTDOWNLATCH_H
