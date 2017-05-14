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

FaceDetector::FaceDetector(int camera_number, 
                 double downsample_ratio, 
                 int sample_rate, 
                 char *face_landmarks_location)
                        : camera_number(camera_number),
                          downsample_ratio(downsample_ratio),
                          sample_rate(sample_rate),
                          face_landmarks_location(face_landmarks_location) { }

FaceDetector::FaceDetector(const FaceDetector& orig) {
    camera_number = orig.camera_number;
    downsample_ratio = orig.downsample_ratio;
    sample_rate = orig.sample_rate;
    face_landmarks_location = orig.face_landmarks_location;
}

FaceDetector::~FaceDetector() { }

std::vector<dlib::image_window::overlay_circle> render_faces (
        const dlib::full_object_detection dets,
        const dlib::rgb_pixel color = dlib::rgb_pixel(0,255,0)
    )
{
    const int radius = 2;
    std::vector<dlib::image_window::overlay_circle> points;
    
    for (unsigned long i = 0; i < dets.num_parts(); ++i)
    {
        dlib::image_window::overlay_circle c(dets.part(i), radius, color);
        points.push_back(c);
    }
    return points;
}

int FaceDetector::start(Writer &writer) {
    
    cv::VideoCapture camera(camera_number);
    if (!camera.isOpened())
    {
        std::cerr << "Writer is unable to connect to camera " << camera_number
                << std::endl;
        return -1;
    }
    
    camera.set(CV_CAP_PROP_FRAME_WIDTH, 
               camera.get(CV_CAP_PROP_FRAME_WIDTH) * (1.0 / downsample_ratio));
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, 
               camera.get(CV_CAP_PROP_FRAME_HEIGHT) * (1.0 / downsample_ratio));
    
    std::cout << "Camera output width: " 
            << camera.get(CV_CAP_PROP_FRAME_WIDTH) 
            << " Camera output height: " << camera.get(CV_CAP_PROP_FRAME_HEIGHT) 
            << std::endl;
    
    std::cout << "Camera max fps: " << camera.get(CV_CAP_PROP_FPS) << std::endl;
    
    std::cout << "Initializing face detection..." << std::endl;
    
    /* dlib face detection variables */
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize(face_landmarks_location) >> sp;
    std::vector<dlib::rectangle> faces;
    dlib::full_object_detection shape;
    dlib::rectangle main_face;
    int face_not_found = 0;

    
    /* dlib graphical window context */
    dlib::image_window win;
    /*
    win.set_size(600, 400);
    dlib::button test(win);
    dlib::menu_bar test2(win);
    
    test2.set_number_of_menus(1);
    test2.set_menu_name(0,"Menu",'M');
    test2.menu(0).add_menu_item(dlib::menu_item_text("Hello!", &test_funct, 0));
    
    test.set_pos(10,300);
    test.set_name("button");
    win.set_title("Stringless");
    */
    
    /* FrameData variable, to be passed into the writer */
    FrameData frame_data;
    const int points = 68;
    
    /* Frame count / frames per second variables */
    int frame_count = 0;
    int frame_count_enter = 0, frame_count_exit;
    int frames_per_second = 0;
    
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

        /*
        cv::Mat resized_frame;
        
        cv::resize(frame, 
                   resized_frame, 
                   cv::Size(), 
                   1.0/downsample_ratio, 
                   1.0/downsample_ratio);
        */
        
        dlib::cv_image<dlib::bgr_pixel> cimg(frame);

        // Detect the faces
	if (frame_count % sample_rate == 0) {
		faces = detector(cimg);
                if (!faces.empty()) {
                    main_face = faces[0];
                    face_not_found = 0;
                }
                else face_not_found++;
	}
 
        // Set main face to be the largest
        for (dlib::rectangle r : faces) {
            if (!main_face.is_empty() && r.area() > main_face.area()) {
                main_face = r;
            }
        }

        if (face_not_found < 5) {
            // Find the pose of face
            shape = sp(cimg, main_face);
            // Push data points into frame data
            for(int i = 0; i < points; ++i) {
                frame_data.points[i].x = (double)shape.part(i).x() / (double)cimg.nr();
                frame_data.points[i].y = (double)shape.part(i).y() / (double)cimg.nc();
            }
        } else shape = dlib::full_object_detection();
        frame_data.fps = frames_per_second;
        frame_data.frame_number = frame_count;
        
        /*
        for (unsigned long i = 0; i < faces.size(); ++i) {            
            dlib::full_object_detection shape = sp(cimg, faces[i]);
            
            // Push data points into frame data
            for(int i = 0; i < points; ++i) {
                frame_data.points[i].x = (double)shape.part(i).x() / (double)cimg.nr();
                frame_data.points[i].y = (double)shape.part(i).y() / (double)cimg.nc();
            }
            frame_data.fps = frames_per_second;
            frame_data.frame_number = frame_count;
            
            shapes.push_back(shape);
        }
        */
        
        std::cout << " Frame rate: " << frames_per_second << " Frame count: " 
                << frame_count << "      \r";
        ++frame_count;
        
        win.clear_overlay();
        win.set_image(cimg);
        win.add_overlay(render_faces(shape));
        
        // Pass frame data to writer
        writer.write(frame_data);

    }
    
    camera.release();
    return 0;
}

} // namespace Stringless