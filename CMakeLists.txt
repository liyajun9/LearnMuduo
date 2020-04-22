cmake_minimum_required(VERSION 3.15)
include(PrecompiledHeader.cmake)
project(LearnMuduo)

set(CMAKE_CXX_STANDARD 11)

add_executable(LearnMuduo main.cpp pch.cpp EchoServer/EchoServer.cpp EchoServer/EchoServer.h net/buffer.cpp net/buffer.h proto/request.pb.h proto/request.pb.cc net/endian.h net/eventLoop.cpp net/eventLoop.h base/thread.cpp base/thread.h base/countdownLatch.cpp base/countdownLatch.h base/threadUtils.cpp base/threadUtils.h test/threadUtilsTest.cpp base/exception.cpp base/exception.h test/threadTest.cpp base/logging.h base/logging.cc base/logStream.h base/logStream.cc base/timestamp.h base/timestamp.cc base/timeZone.h base/timeZone.cc base/stringPiece.h base/typeUtils.h base/date.h base/date.cc base/convertFormatUtils.cpp base/convertFormatUtils.h net/channel.cpp net/channel.h net/poller.cpp net/poller.h net/timerQueue.cpp net/timerQueue.h net/timer.cpp net/timer.h net/timerId.h base/systemUtils.cpp base/systemUtils.h net/socketUtils.cpp net/socketUtils.h net/eventLoopThread.cpp net/eventLoopThread.h net/acceptor.cpp net/acceptor.h net/socket.cpp net/socket.h net/inetAddress.cpp net/inetAddress.h net/tcpServer.cpp net/tcpServer.h net/tcpConnection.cpp net/tcpConnection.h net/alias.h base/alias.h net/eventLoopThreadPool.cpp net/eventLoopThreadPool.h net/poller/pollPoller.cpp net/poller/pollPoller.h net/poller/epollPoller.cpp net/poller/epollPoller.h net/protobuf/protobufCodecLite.cpp net/protobuf/protobufCodecLite.h base/threadPool.cpp base/threadPool.h applications/protobuf/codec/server.cpp applications/protobuf/codec/server.h applications/protobuf/codec/protobufCodec.cpp applications/protobuf/codec/protobufCodec.h applications/protobuf/codec/dispatcher_lite.cpp applications/protobuf/codec/dispatcher_lite.h)

add_precompiled_header(LearnMuduo pch.h FORCEINCLUDE)

#if(CMAKE_BUILD_TYPE AND NOT Debug)
#    target_include_directories(LearnMuduo PUBLIC libs/muduo/debug/include)
#    target_link_directories(LearnMuduo PUBLIC libs/muduo/debug/lib)
#else()
#    target_include_directories(LearnMuduo PUBLIC libs/muduo/release/include)
#    target_link_directories(LearnMuduo PUBLIC libs/muduo/release/lib)
#endif()

target_link_libraries(LearnMuduo PUBLIC -pthread)
target_link_libraries(LearnMuduo PUBLIC -lprotobuf)
target_link_libraries(LearnMuduo PUBLIC -lgtest)
target_link_libraries(LearnMuduo PUBLIC -lz)

#target_link_libraries(LearnMuduo PUBLIC -lmuduo_base)
#target_link_libraries(LearnMuduo PUBLIC -lmuduo_net)
#target_link_libraries(LearnMuduo PUBLIC -lmuduo_curl)
#target_link_libraries(LearnMuduo PUBLIC -lmuduo_http)
#target_link_libraries(LearnMuduo PUBLIC -lmuduo_inspect)