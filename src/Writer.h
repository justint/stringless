/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   Writer.h
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
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
    
    int write(FrameData frame_data);
private:
    FrameData *data_address;
    Mutex mutex;
    
    FrameData first_frame;
    bool second_frame;
};

} // namespace Stringless

#endif /* WRITER_H */

