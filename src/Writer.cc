/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   Writer.cc
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 * 
 * Created on February 2, 2017, 11:31 AM
 */

#include "Writer.h"

namespace Stringless {
/*
 * Constructs the Writer, for writing into shared memory.
 * 
 * First argument is the address to the shared memory where the frame data
 * will be written to, the second argument is the camera to be selected for 
 * grabbing frames and then processed in OpenFace.
 */
Writer::Writer(FrameData *data_address) 
    : data_address(data_address),
      second_frame(false) {
    this->mutex.init();
}

Writer::Writer(const Writer& orig) {
    data_address = orig.data_address;
    mutex = orig.mutex;
}

Writer::~Writer() {
    delete data_address;
}

int Writer::write(FrameData frame_data) {
    // Write both the saved first frame and the incoming frame if the incoming 
    // frame is the second of the pair
    if (second_frame) {
        
        mutex.lock();
        data_address[0] = first_frame;
        data_address[1] = frame_data;
        mutex.unlock();
        
        second_frame = false;
    }
    // Save the incoming frame as the first of the pair
    else {
        first_frame = frame_data;
        second_frame = true;
    }
    return 0;
}

} // namespace Stringless