/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   FaceDetector.cc
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 * 
 * Created on February 4, 2017, 11:04 PM
 */



#include "FaceDetector.h"

#include <chrono>
#include <iostream>
#include <string>
#include <utility>

#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/image_processing/render_face_detections.h"
#include "dlib/image_processing.h"
#include "dlib/gui_widgets.h"
#include "dlib/opencv.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace Stringless {

FaceDetector::FaceDetector(int camera_number, char *face_landmarks_location) 
                        : camera_number(camera_number),
                          face_landmarks_location(face_landmarks_location) { }

FaceDetector::FaceDetector(const FaceDetector& orig) {
    camera_number = orig.camera_number;
    face_landmarks_location = orig.face_landmarks_location;
}

FaceDetector::~FaceDetector() {
    delete face_landmarks_location;
}

int FaceDetector::start(Writer &writer) {
    
    cv::VideoCapture camera(camera_number);
    if (!camera.isOpened())
    {
        std::cerr << "Writer is unable to connect to camera " << camera_number
                << std::endl;
        return -1;
    }
    
    std::cout << "Initializing face detection..." << std::endl;
    
    /* dlib face detection variables */
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize(face_landmarks_location) >> sp;
    
    /* dlib graphical window context */
    dlib::image_window win;
    
    const int downsample_ratio = 2;
    
    /* FrameData variable, to be passed into the writer */
    FrameData frame_data;
    const int points = 68;
    
    /* Frame count / frames per second variables */
    int frame_count = 0;
    int frame_count_enter = 0, frame_count_exit;
    int frames_per_second;
    
    auto start = std::chrono::steady_clock::now();
    while(!win.is_closed())
    {
        // Framerate calculations
        auto diff = std::chrono::steady_clock::now() - start;
        frame_count_exit = frame_count;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() >= 1000)
        {
            frames_per_second = frame_count_exit - frame_count_enter;
            frame_count_enter = frame_count_exit;
            start = std::chrono::steady_clock::now();
            
        }
        
        // Take image from camera
        cv::Mat frame;
        camera >> frame;
        
        cv::Mat resized_frame;
        
        cv::resize(frame, 
                   resized_frame, 
                   cv::Size(), 
                   1.0/downsample_ratio, 
                   1.0/downsample_ratio);
        
        dlib::cv_image<dlib::bgr_pixel> cimg(resized_frame);
        
        // Detect the faces
        std::vector<dlib::rectangle> faces = detector(cimg);
 
        // Find the pose of each face
        std::vector<dlib::full_object_detection> shapes;
        for (unsigned long i = 0; i < faces.size(); ++i)
        {
            /*
            dlib::rectangle r(
                        (long)(faces[i].left() * downsample_ratio),
                        (long)(faces[i].top() * downsample_ratio),
                        (long)(faces[i].right() * downsample_ratio),
                        (long)(faces[i].bottom() * downsample_ratio)
            ); 
            */
            dlib::full_object_detection shape = sp(cimg, faces[i]);
            //dlib::full_object_detection shape = sp(cimg, r);
            
            // Push data points into frame data
            for(int i = 0; i < points; ++i)
            {
                frame_data.points[i].x = shape.part(i).x();
                frame_data.points[i].y = shape.part(i).y();
            }
            frame_data.fps = frames_per_second;
            
            shapes.push_back(shape);
        }
        
        std::cout << " Frame rate: " << frames_per_second << " Frame count: " << frame_count << "      \r";
        ++frame_count;
        
        win.clear_overlay();
        win.set_image(cimg);
        win.add_overlay(dlib::render_face_detections(shapes));
        
        // Pass frame data to writer
        writer.write(frame_data);

    }
    
    return 0;
}

} // namespace Stringless