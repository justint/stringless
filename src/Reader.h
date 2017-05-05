/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   Reader.h
 * Author: jtennant
 *
 * Created on February 2, 2017, 12:22 PM
 */

#ifndef READER_H
#define READER_H


#include "FrameData.h"
#include "Mutex.h"

namespace Stringless {

class Reader {
public:
    Reader();
    Reader(FrameData *data_address);
    Reader(const Reader& orig);
    virtual ~Reader();
    
    FrameData * read();
private:
    FrameData *data_address;
    Mutex mutex;
};

} // namespace Stringless

#endif /* READER_H */

