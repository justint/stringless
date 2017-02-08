/* 
 * File:   Reader.h
 * Author: jtennant
 *
 * Created on February 2, 2017, 12:22 PM
 */

#ifndef READER_H
#define READER_H



#include "../FrameData.h"
#include "../Mutex.h"

namespace Stringless {

class Reader {
public:
    Reader(FrameData *data_address);
    Reader(const Reader& orig);
    virtual ~Reader();
    
    FrameData const * const read();
private:
    FrameData *data_address;
    Mutex mutex;
};

} // namespace Stringless

#endif /* READER_H */

