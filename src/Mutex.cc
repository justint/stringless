/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   Mutex.cc
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 * 
 * Created on February 2, 2017, 11:51 AM
 */

#include "Mutex.h"

namespace Stringless {

void Mutex::init() {
    int err = pthread_mutexattr_init(&attr);
    if (err) throw err;
    
    err = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    if (err) throw err;
    
    err = pthread_mutex_init(&_mutex, &attr);
    if (err) throw err;
}

void Mutex::lock() {
    int err = pthread_mutex_lock(&_mutex);
    if (err) throw err;
}

void Mutex::unlock() {
    int err = pthread_mutex_unlock(&_mutex);
    if (err) throw err;
}

} // namespace Stringless