/* 
 * File:   Writer.h
 * Author: jtennant
 *
 * Created on February 2, 2017, 11:31 AM
 */

#ifndef WRITER_H
#define WRITER_H

#include "FrameData.h"
#include "Mutex.h"

namespace Stringless {

class Writer {
public:
    Writer(FrameData *data_address);
    Writer(const Writer& orig);
    virtual ~Writer();
    
    int start();
    int stop();
private:
    FrameData *data_address;
    Mutex mutex;
};

} // namespace Stringless

#endif /* WRITER_H */

