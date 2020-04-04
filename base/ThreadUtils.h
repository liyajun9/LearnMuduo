//
// Created by ralph on 4/4/20.
//

#ifndef LINUXSERVER_THREADUTILS_H
#define LINUXSERVER_THREADUTILS_H

namespace ybase{

extern __thread int tls_cachedTid;         //thread's LWP process id
extern __thread char tls_tidString[32];    //tid in string
extern __thread int tls_tidStringLength;   //length of tid in string
extern __thread const char* tls_threadName;//thread name

class ThreadUtils {

public:
    static int tid(){
        if(__builtin_expect((tls_cachedTid == 0), 0)){ //optimize if else
            cacheTid();
        }
        return tls_cachedTid;
    }

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

    std::string stackTrace(bool demangle);

private:
    static void cacheTid(){
        if(tls_cachedTid == 0){
            tls_cachedTid = getTid();
            tls_tidStringLength = snprintf(tls_tidString, sizeof(tls_tidString), "5d ", tls_cachedTid);
        }
    }

    static pid_t getTid(); //system call
};

} //namespace ybase



#endif //LINUXSERVER_THREADUTILS_H
