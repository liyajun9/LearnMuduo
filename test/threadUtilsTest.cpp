//
// Created by ralph on 4/4/20.
//
#include <gtest/gtest.h>
#include "../base/threadUtils.h"

TEST(ThreadUtilsTest, mainThreadInitializer){
    std::cout << "cachedTid: " << ybase::tls_cachedTid << std::endl;
    std::cout << "tidString: " << ybase::tls_tidString << std::endl;
    std::cout << "StringLength: " << ybase::tls_tidStringLength << std::endl;
    std::cout << "threadName: " << ybase::tls_threadName << std::endl;
    EXPECT_NE(ybase::tls_cachedTid, 0);
    EXPECT_STRNE(ybase::tls_tidString, "unknown");
    EXPECT_EQ(ybase::tls_tidStringLength, 6);
    EXPECT_STREQ(ybase::tls_threadName, "mainThread");
}

