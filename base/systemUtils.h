//
// Created by ralph on 4/13/20.
//

#ifndef LEARNMUDUO_SYSTEMUTILS_H
#define LEARNMUDUO_SYSTEMUTILS_H

namespace ybase {

class SystemUtils {
    NonCopyable(SystemUtils)

public:
    static int createTimerfd();
    static int createEventfd();
};

} //namespace ybase
#endif //LEARNMUDUO_SYSTEMUTILS_H
