//
// Created by ralph on 4/4/20.
//

#include <sys/syscall.h>
#include <unistd.h>
#include <execinfo.h>
#include <cxxabi.h>
#include "ThreadUtils.h"

namespace ybase {

__thread int tls_cachedTid = 0;
__thread char tls_tidString[32];        //tid in string
__thread int tls_tidStringLength = 6;
__thread const char* tls_threadName = "unknown";

pid_t ThreadUtils::getTid() {
    return static_cast<pid_t>(syscall(SYS_gettid));
}

bool ThreadUtils::isMainThread() {
    return tid() == ::getpid();
}

std::string ThreadUtils::stackTrace(bool demangle) {
    std::string stack;
    constexpr int max_frames = 200;
    void* frame[max_frames];
    int nptrs = ::backtrace(frame, max_frames);
    char** strings = ::backtrace_symbols(frame, max_frames);
    if(strings){
        size_t len = 256;
        char* demangled = demangled ? static_cast<char*>(::malloc(len)) : nullptr;
        for(int i = 1; i < nptrs; ++i){
            if(demangled){
                char* left_par = nullptr;
                char* plus = nullptr;
                for (char* p = strings[i]; *p; ++p)
                {
                    if (*p == '(')
                        left_par = p;
                    else if (*p == '+')
                        plus = p;
                }

                if (left_par && plus) {
                    *plus = '\0';
                    int status = 0;
                    char *ret = abi::__cxa_demangle(left_par + 1, demangled, &len, &status);
                    *plus = '+';
                    if (status == 0) {
                        demangled = ret;  // ret could be realloc()
                        stack.append(strings[i], left_par + 1);
                        stack.append(demangled);
                        stack.append(plus);
                        stack.push_back('\n');
                        continue;
                    }
                }
            }
            stack.append(strings[i]);
            stack.push_back('\n');
        }
        free(demangled);
        free(strings);
    }
    return stack;
}

} //namespace ybase