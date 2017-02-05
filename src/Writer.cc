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

#include <chrono>
#include <iostream>
#include <string>

#include "dlib/image_processing/frontal_face_detector.h"
#include "dlib/image_processing/render_face_detections.h"
#include "dlib/image_processing.h"
#include "dlib/gui_widgets.h"
#include "dlib/opencv.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace Stringless {
/*
 * Constructs the Writer, for writing into shared memory.
 * 
 * First argument is the address to the shared memory where the frame data
 * will be written to, the second argument is the camera to be selected for 
 * grabbing frames and then processed in OpenFace.
 */
Writer::Writer(FrameData *data_address, 
               int camera_number, 
               char *face_landmarks_location) {
    this->data_address = data_address;
    this->camera_number = camera_number;
    this->face_landmarks_location = face_landmarks_location;
    this->mutex.init();
}

Writer::Writer(const Writer& orig) {
    data_address = orig.data_address;
    camera_number = orig.camera_number;
    face_landmarks_location = orig.face_landmarks_location;
    mutex = orig.mutex;
}

Writer::~Writer() {
    delete data_address;
    delete face_landmarks_location;
}

/*
 * Initializes the shared memory writing process.
 * 
 * With the given address from construction, the Writer will use dlib to draw
 * facial landmark data from two camera frame captures at a time and write them 
 * both to the shared memory.
 * 
 * This process (should) be stopped using the stop() call. Currently
 * threading hasn't been implemented so stop() logically should never be reached
 * (process control should enter start() and continue to run until halted by the
 * OS).
 * 
 * Returns 0 if successful, -1 if camera opening fails.
 */
int Writer::start() {
    
    cv::VideoCapture camera(camera_number);
    if (!camera.isOpened())
    {
        std::cerr << "Writer is unable to connect to camera " << camera_number
                << std::endl;
        return -1;
    }
    
    //cv::Mat edges;
    //cv::namedWindow("edges", 1);
    
    std::cout << "Initializing face detection..." << std::endl;
    
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize(face_landmarks_location) >> sp;
    
    dlib::image_window win;
    
    const int downsample_ratio = 2;
    const int frame_skip = 2;
    
    int frame_count = 0;
    int frame_count_enter = 0, frame_count_exit;
    int frames_per_second;
    
    auto start = std::chrono::steady_clock::now();
    while(!win.is_closed())
    {
        auto diff = std::chrono::steady_clock::now() - start;
        frame_count_exit = frame_count;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() >= 1000)
        {
            frames_per_second = frame_count_exit - frame_count_enter;
            frame_count_enter = frame_count_exit;
            start = std::chrono::steady_clock::now();
            
        }
        
        cv::Mat frame;
        camera >> frame;
        
        cv::Size size = frame.size();
        cv::Mat resized_frame;
        
        cv::resize(frame, resized_frame, cv::Size(), 1.0/downsample_ratio, 1.0/downsample_ratio);
        
        dlib::cv_image<dlib::bgr_pixel> cimg(resized_frame);
        
        // Detect the faces
        std::vector<dlib::rectangle> faces = detector(cimg);
 
        // Find the pose of each face
        std::vector<dlib::full_object_detection> shapes;
        for (unsigned long i = 0; i < faces.size(); ++i)
        {
            dlib::rectangle r(
                        (long)(faces[i].left() * 1.0/downsample_ratio),
                        (long)(faces[i].top() * 1.0/downsample_ratio),
                        (long)(faces[i].right() * 1.0/downsample_ratio),
                        (long)(faces[i].bottom() * 1.0/downsample_ratio)
            ); 
            dlib::full_object_detection shape = sp(cimg, faces[i]);
            //std::cout << "number of parts: "<< shape.num_parts() << std::endl;
            //std::cout << "pixel position of first part:  " << shape.part(0) << std::endl;
            //std::cout << "pixel position of second part: " << shape.part(1) << std::endl;
            shapes.push_back(shape);
        }
        
        std::cout << " Frame rate: " << frames_per_second << " Frame count: " << frame_count << "      \r";
        ++frame_count;
        
        win.clear_overlay();
        win.set_image(cimg);
        win.add_overlay(dlib::render_face_detections(shapes));
        
        /*
        if (!frame.empty()) {
            cv::imshow("edges", edges);
            ++frame_count;
        }
        */
        //cv::waitKey(16); // 60 fps maximum

    }
    
    return 0;
}

/*
 * Halts the shared memory writing process. (NOT IMPLEMENTED YET)
 * 
 * TODO: Set up threading in Stringless.cc so that stop() calls can be reached.
 */
int Writer::stop() {
    
}

} // namespace Stringless