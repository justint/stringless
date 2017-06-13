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

#include "dlib/gui_widgets.h"

namespace Stringless {

class FaceDetector {
public:
    FaceDetector(int    camera_number, 
                 int    width,
                 int    height,
                 int    fps,
                 double downsample_ratio, 
                 int    sample_rate, 
                 int    landmark_sample_per_frame,
                 char   *face_landmarks_location);
    FaceDetector(const FaceDetector& orig);
    virtual ~FaceDetector();
    
    int start(Writer &writer);
private:
    int                 camera_number;
    int                 width;
    int                 height;
    int                 fps;
    double              downsample_ratio;
    int                 sample_rate;
    int                 landmark_sample_per_frame;
    char                *face_landmarks_location;
    
    bool                show_camera_image;
    unsigned int        neutral_face_flag;                
    dlib::image_window  win;
    
    void                toggle_camera_image();
    void                capture_neutral_face();
    void                show_neutral_deltas();
    void                reset_neutral_face();
    
    const unsigned int  RESET_NEUTRAL_FACE      = 0;
    const unsigned int  SHOW_NEUTRAL_FACE       = 1;
    const unsigned int  CAPTURE_NEUTRAL_FACE    = 2;
    const unsigned int  SHOW_NEUTRAL_DELTAS     = 3;

};

} // namespace Stringless

#endif /* FACEDETECTOR_H */

