/* 
 * File:   Writer.cc
 * Author: jtennant
 * 
 * Created on February 2, 2017, 11:31 AM
 */

#include "Writer.h"

namespace Stringless {

Writer::Writer(FrameData *data_address) {
    this->data_address = data_address;
    this->mutex.init();
}

Writer::Writer(const Writer& orig) {
    data_address = orig.data_address;
    mutex = orig.mutex;
}

} // namespace Stringless