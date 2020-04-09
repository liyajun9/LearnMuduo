//
// Created by ralph on 4/4/20.
//

#ifndef LINUXSERVER_THREADUTILS_H
#define LINUXSERVER_THREADUTILS_H

#include <cassert>

namespace ybase{

/* thread local storage
 * cachedTid:       thread's LWP pid
 * tidString:       cachedTid in string
 * tidStringLength: cachedTid length in string
 * threadName:      user defined thread getName
 */
extern __thread int tls_cachedTid;         //thread's LWP process id
extern __thread char tls_tidString[32];    //getTid in string
extern __thread int tls_tidStringLength;   //length of getTid in string
extern __thread const char* tls_threadName;//thread getName

class ThreadUtils {
    class MainThreadTLSInitializer{
    public:
        MainThreadTLSInitializer();
        static void resetTLSAfterFork();
    };

public:
    static int getTid();

    static const char* tidString(){
        return tls_tidString;
    }

    static int tidStringLength(){
        return tls_tidStringLength;
    }

    static const char* name(){
        return tls_threadName;
    }

    static bool isMainThread();

    static std::string stackTrace(bool demangle);

private:
    static void cacheTid(){
        if(tls_cachedTid == 0){
            tls_cachedTid = getSysTid();
            tls_tidStringLength = snprintf(tls_tidString, sizeof(tls_tidString), "%5d ", tls_cachedTid);
        }
    }

    static pid_t getSysTid(); //system call

    static MainThreadTLSInitializer mainThreadTlsInitializer;
};


} //namespace ybase



#endif //LINUXSERVER_THREADUTILS_H
