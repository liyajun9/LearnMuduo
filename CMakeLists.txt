cmake_minimum_required(VERSION 3.15)
include(PrecompiledHeader.cmake)
project(LinuxServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(LinuxServer main.cpp pch.cpp EchoServer/EchoServer.cpp EchoServer/EchoServer.h net/buffer.cpp net/buffer.h proto/request.pb.h proto/request.pb.cc net/endian.h net/eventLoop.cpp net/eventLoop.h base/thread.cpp base/thread.h base/countdownLatch.cpp base/countdownLatch.h base/threadUtils.cpp base/threadUtils.h test/threadUtilsTest.cpp base/exception.cpp base/exception.h test/threadTest.cpp base/logging.h base/logging.cc base/logStream.h base/logStream.cc base/timestamp.h base/timestamp.cc base/timeZone.h base/timeZone.cc base/stringPiece.h base/typeUtils.h base/date.h base/date.cc base/convertFormatUtils.cpp base/convertFormatUtils.h net/channel.cpp net/channel.h net/poller.cpp net/poller.h net/timerQueue.cpp net/timerQueue.h net/timer.cpp net/timer.h net/timerId.h)

add_precompiled_header(LinuxServer pch.h FORCEINCLUDE)

#if(CMAKE_BUILD_TYPE AND NOT Debug)
#    target_include_directories(LinuxServer PUBLIC libs/muduo/debug/include)
#    target_link_directories(LinuxServer PUBLIC libs/muduo/debug/lib)
#else()
#    target_include_directories(LinuxServer PUBLIC libs/muduo/release/include)
#    target_link_directories(LinuxServer PUBLIC libs/muduo/release/lib)
#endif()

target_link_libraries(LinuxServer PUBLIC -pthread)
target_link_libraries(LinuxServer PUBLIC -lprotobuf)
target_link_libraries(LinuxServer PUBLIC -lgtest)

#target_link_libraries(LinuxServer PUBLIC -lmuduo_base)
#target_link_libraries(LinuxServer PUBLIC -lmuduo_net)
#target_link_libraries(LinuxServer PUBLIC -lmuduo_curl)
#target_link_libraries(LinuxServer PUBLIC -lmuduo_http)
#target_link_libraries(LinuxServer PUBLIC -lmuduo_inspect)