//
// Created by ralph on 4/4/20.
//

#include <sys/syscall.h>
#include <unistd.h>
#include <execinfo.h>
#include <cxxabi.h>
#include "threadUtils.h"

namespace ybase {

thread_local int tls_cachedTid = 0;
thread_local char tls_tidString[32];
thread_local int tls_tidStringLength = 6;
thread_local const char* tls_threadName = "unknown";

ThreadUtils::MainThreadTLSInitializer ThreadUtils::mainThreadTlsInitializer; //initialize tls of main thread

ThreadUtils::MainThreadTLSInitializer::MainThreadTLSInitializer(){
    assert(tls_cachedTid == 0);
    tls_threadName = "mainThread";
    ThreadUtils::getTid();
    pthread_atfork(nullptr, nullptr, resetTLSAfterFork);
}

void ThreadUtils::MainThreadTLSInitializer::resetTLSAfterFork(){
    tls_cachedTid = 0;
    tls_threadName = "mainThread";
    ThreadUtils::getTid();
}

int ThreadUtils::getTid(){
    if(__builtin_expect((tls_cachedTid == 0), 0)){ //optimize if else
        cacheTid();
    }
    return tls_cachedTid;
}

pid_t ThreadUtils::getSysTid() {
    return static_cast<pid_t>(syscall(SYS_gettid));
}

bool ThreadUtils::isMainThread() {
    return getTid() == ::getpid();
}

std::string ThreadUtils::stackTrace(bool demangle) {
    std::string stack;
    constexpr int max_frames = 200;
    void* frame[max_frames];
    int nptrs = ::backtrace(frame, max_frames);
    char** strings = ::backtrace_symbols(frame, max_frames);
    if(strings){
        size_t len = 256;
        char* demangled = demangle ? static_cast<char*>(::malloc(len)) : nullptr;
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