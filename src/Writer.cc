/* 
 * File:   Writer.cc
 * Author: jtennant
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
Writer::Writer(FrameData *data_address) {
    this->data_address = data_address;
    this->mutex.init();
}

Writer::Writer(const Writer& orig) {
    data_address = orig.data_address;
    mutex = orig.mutex;
}

Writer::~Writer() {
    delete data_address;
}

/*
 * Initializes the shared memory writing process.
 * 
 * With the given address from construction, the Writer will use its OpenFacer
 * instance to draw facial landmark data from two camera frame captures at a
 * time and write them both to the shared memory.
 * 
 * This process (should) be stopped using the stop() call. Currently
 * threading hasn't been implemented so stop() logically should never be reached
 * (process control should enter start() and continue to run until halted by the
 * OS).
 */
int Writer::start() {
    
}

/*
 * Halts the shared memory writing process. (NOT IMPLEMENTED YET)
 * 
 * TODO: Set up threading in Stringless.cc so that stop() calls can be reached.
 */
int Writer::stop() {
    
}

} // namespace Stringless