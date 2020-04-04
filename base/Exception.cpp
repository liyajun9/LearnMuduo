//
// Created by ralph on 4/4/20.
//

#include "Exception.h"
#include "ThreadUtils.h"

namespace ybase{

Exception::Exception(const char *message /*= ""*/) noexcept
        : m_message(message), m_stackTrace(ThreadUtils::stackTrace(false)) {
}

}
