//
// Created by ralph on 4/13/20.
//

#ifndef LINUXSERVER_SYSTEMUTILS_H
#define LINUXSERVER_SYSTEMUTILS_H

namespace ybase {

class SystemUtils {
    NonCopyable(SystemUtils)

public:
    static int createTimerfd();
    static int createEventfd();
};

} //namespace ybase
#endif //LINUXSERVER_SYSTEMUTILS_H
