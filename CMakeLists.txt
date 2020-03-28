cmake_minimum_required(VERSION 3.15)
include(PrecompiledHeader.cmake)
project(LinuxServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(LinuxServer main.cpp pch.cpp pch.h EchoServer/EchoServer.cpp EchoServer/EchoServer.h)

add_precompiled_header(LinuxServer pch.h FORCEINCLUDE)

if(CMAKE_BUILD_TYPE AND NOT Debug)
    target_include_directories(LinuxServer PUBLIC libs/muduo/debug/include)
    target_link_directories(LinuxServer PUBLIC libs/muduo/debug/lib)
else()
    target_include_directories(LinuxServer PUBLIC libs/muduo/release/include)
    target_link_directories(LinuxServer PUBLIC libs/muduo/release/lib)
endif()

target_link_libraries(LinuxServer PUBLIC -pthread)
target_link_libraries(LinuxServer PUBLIC -lmuduo_base)
target_link_libraries(LinuxServer PUBLIC -lmuduo_net)
target_link_libraries(LinuxServer PUBLIC -lmuduo_curl)
target_link_libraries(LinuxServer PUBLIC -lmuduo_http)
target_link_libraries(LinuxServer PUBLIC -lmuduo_inspect)