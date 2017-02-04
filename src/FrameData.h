/* 
 * File:   FrameData.h
 * Author: jtennant
 *
 * Created on February 2, 2017, 12:29 PM
 */

#ifndef FRAMEDATA_H
#define FRAMEDATA_H

namespace Stringless {

struct FrameData {
    FrameData();
    FrameData(const FrameData& orig);
    virtual ~FrameData();
    
    int length;
    char *buffer;
};

} // namespace Stringless

#endif /* FRAMEDATA_H */

