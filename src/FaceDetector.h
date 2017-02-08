/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   FaceDetector.h
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 *
 * Created on February 4, 2017, 11:04 PM
 */

#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include "Writer.h"

namespace Stringless {

class FaceDetector {
public:
    FaceDetector(int camera_number, char *face_landmarks_location);
    FaceDetector(const FaceDetector& orig);
    virtual ~FaceDetector();
    
    int start(Writer &writer);
private:
    int camera_number;
    char *face_landmarks_location;

};

} // namespace Stringless

#endif /* FACEDETECTOR_H */

