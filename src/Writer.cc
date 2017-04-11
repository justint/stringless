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
Writer::Writer(void * data_address) 
    : data_address(data_address),
      is_second_frame(false),
      initial_write(true) {
    this->mutex.init();
}

Writer::Writer(const Writer& orig) {
    data_address    = orig.data_address;
    is_second_frame = orig.is_second_frame;
    initial_write   = orig.initial_write;
    mutex = orig.mutex;
}

Writer::~Writer() { }

int Writer::write(FrameData &incoming_frame_data) {
    // Initialize the memory location if this is the first time writing
    if (initial_write)
    {
        mutex.lock();
        //first_frame = new (data_address) FrameData;
        //second_frame = new (++data_address) FrameData;
        //first_frame = &frame_data;
        frame_data = (FrameData*) data_address;
        mutex.unlock();
        
        initial_write = false;
        is_second_frame = true;
        return 0;
    }
    // Write both the saved first frame and the incoming frame if the incoming 
    // frame is the second of the pair
    if (is_second_frame) {
        
        mutex.lock();
        second_frame = incoming_frame_data;
        frame_data[0] = first_frame;
        frame_data[1] = second_frame;  
        mutex.unlock();
        
        is_second_frame = false;
    }
    // Save the incoming frame as the first of the pair
    else {
        mutex.lock();
        first_frame = incoming_frame_data;
        mutex.unlock();
        
        is_second_frame = true;
    }
    return 0;
}

} // namespace Stringless