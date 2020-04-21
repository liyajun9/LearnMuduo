#include "EchoServer/EchoServer.h"
#include "base/logging.h"
#include "net/eventLoop.h"
#include "base/thread.h"
#include <climits>
#include <set>
#include <google/protobuf/message.h>
#include <unistd.h>
#include <gtest/gtest.h>

int main() {

    char text[] = "1234";
    std::cout << text << std::endl;

    auto func = [](){
        ynet::EventLoop loop;
        loop.loop();
    };

    ybase::Thread thread1(func);
    ybase::Thread thread2(func);

    thread1.start();
    thread2.start();

    thread1.join();
    thread2.join();

//    ::testing::InitGoogleTest();
//    RUN_ALL_TESTS();
    return 0;
}