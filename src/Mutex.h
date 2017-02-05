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

namespace Stringless {

class Mutex {
public:
    Mutex();
    Mutex(const Mutex& orig);
    virtual ~Mutex();
    
    void init();
    void lock();
    void unlock();
    
private:
    int err;
};

} // namespace Stringless

#endif /* MUTEX_H */

