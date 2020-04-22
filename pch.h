//
// Created by ralph on 3/25/20.
//

#ifndef LEARNMUDUO_PCH_H
#define LEARNMUDUO_PCH_H

#define NonCopyable(ClassName) ClassName(const ClassName& ) = delete; \
                                ClassName& operator =(const ClassName&) = delete;

#include <iostream>
#include <string>

#define LOGTO_STDOUT
#endif //LEARNMUDUO_PCH_H
