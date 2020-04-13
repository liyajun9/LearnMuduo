//
// Created by ralph on 3/25/20.
//

#ifndef LINUXSERVER_PCH_H
#define LINUXSERVER_PCH_H

#define NonCopyable(ClassName) ClassName(const ClassName& ) = delete; \
                                ClassName& operator =(const ClassName&) = delete;

#include <iostream>
#include <string>

using TimerCallback = std::function<void()>;

#define LOGTO_STDOUT
#endif //LINUXSERVER_PCH_H
