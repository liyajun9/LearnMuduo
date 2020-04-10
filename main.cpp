#include "EchoServer/EchoServer.h"
#include "base/logging.h"
#include <climits>
#include <set>
#include <google/protobuf/message.h>
#include <unistd.h>
#include <gtest/gtest.h>

int main() {
    LOG_INFO <<  "show log1\n";
    LOG_INFO <<  "show log2\n";

    ::testing::InitGoogleTest();
    RUN_ALL_TESTS();
    return 0;
}