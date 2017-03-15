/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   Mutex.h
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 *
 * Created on February 2, 2017, 11:51 AM
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

namespace Stringless {

class Mutex {
public:
    Mutex() {};
    Mutex(const Mutex& orig) {
        _mutex = orig._mutex;
        attr = orig.attr;
    };
    virtual ~Mutex() {};
    
    void init();
    void lock();
    void unlock();
    
private:
    pthread_mutex_t _mutex;
    pthread_mutexattr_t attr;
};

} // namespace Stringless

#endif /* MUTEX_H */

