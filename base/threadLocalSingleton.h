//
// Created by ralph on 4/23/20.
//

#ifndef LEARNMUDUO_THREADLOCALSINGLETON_H
#define LEARNMUDUO_THREADLOCALSINGLETON_H

#include <zconf.h>

namespace ybase {

template <typename T>
class ThreadLocalSingleton {
    ThreadLocalSingleton() = delete;
    ~ThreadLocalSingleton() = delete;

class Deleter{
public:
    static T& instance(){
        if(!m_value){
            m_value = new T();
            m_deleter.set(m_value);
        }
        return *m_value;
    }

    static T* pointer(){
        return m_value;
    }

private:

    static void destructor(void* obj){
        assert(obj == m_value);
        delete m_value;
        m_value = 0;
    }
public:
    Deleter(){
        pthread_key_create(&m_pkey, &destructor);
    }

    ~Deleter(){
        pthread_key_delete(m_pkey);
    }

    void set(T* newObj){
        assert(pthread_getspecific(m_pkey) == nullptr);
        pthread_setspecific(m_pkey, newObj);
    }

    pthread_key_t m_pkey;
};

private:
    static thread_local T* m_value;
    static Deleter m_deleter;
};

} //namespace ybase

#endif //LEARNMUDUO_THREADLOCALSINGLETON_H
