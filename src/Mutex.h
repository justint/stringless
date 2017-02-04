/* 
 * File:   Mutex.h
 * Author: jtennant
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

