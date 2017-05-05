/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   FrameData.h
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 *
 * Created on February 2, 2017, 12:29 PM
 */

#ifndef FRAMEDATA_H
#define FRAMEDATA_H

namespace Stringless {
    
struct PointPair {
    double x;
    double y;
};

struct FrameData {
    PointPair points[68];
    int fps;
    int frame_number;
};

} // namespace Stringless

#endif /* FRAMEDATA_H */

