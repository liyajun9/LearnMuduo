//
// Created by ralph on 4/4/20.
//

#include "../base/thread.h"
#include "../base/threadUtils.h"
#include "../base/exception.h"
#include <gtest/gtest.h>

using namespace ybase;

class ThreadFixture : public ::testing::Test{
public:
    void SetUp() override{
        auto func = [](){
            std::cout << "start - " << ThreadUtils::getThreadName() << " : " << ThreadUtils::getTid() << std::endl;
        };
        for(int i = 0; i < 10; ++i){
            std::unique_ptr<Thread> up(new Thread(func));
            up->start();
            threads.push_back(std::move(up));
        }
    }

    void TearDown() override{
        for(int i = 0; i < 10; ++i){
            threads.pop_back();
        }
    }
    std::vector<std::unique_ptr<Thread> > threads;
};

//ctor, dtor, start, join, started, getTid, getName, exception
TEST_F(ThreadFixture, ctor){

}

TEST_F(ThreadFixture, join){

    for(auto iter = threads.begin(); iter != threads.end(); ++iter){
        std::cout << (*iter)->getName() << " join result" << (*iter)->join() << std::endl;
    }
}