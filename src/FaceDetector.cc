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
                 int width,
                 int height,
                 int fps,
                 double downsample_ratio, 
                 int sample_rate, 
                 int landmark_sample_per_frame,
                 char *face_landmarks_location)
                        : camera_number(camera_number),
                          width(width),
                          height(height),
                          fps(fps),
                          downsample_ratio(downsample_ratio),
                          sample_rate(sample_rate),
                          landmark_sample_per_frame(landmark_sample_per_frame),
                          face_landmarks_location(face_landmarks_location),
                          show_camera_image(false),
                          neutral_face_flag(0),
                          win() { }

FaceDetector::FaceDetector(const FaceDetector& orig) {
    camera_number               = orig.camera_number;
    width                       = orig.width;
    height                      = orig.width;
    fps                         = orig.fps;
    downsample_ratio            = orig.downsample_ratio;
    sample_rate                 = orig.sample_rate;
    landmark_sample_per_frame   = orig.landmark_sample_per_frame;
    face_landmarks_location     = orig.face_landmarks_location;
    show_camera_image           = orig.show_camera_image;
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

std::vector<dlib::image_window::overlay_line> render_deltas (
        const dlib::full_object_detection face_origin,
        const dlib::full_object_detection face_dest,
        const dlib::rgb_pixel color = dlib::rgb_pixel(0,0,255)
    )
{
    std::vector<dlib::image_window::overlay_line> lines;
    
    for (unsigned long i = 0; i < face_origin.num_parts(); ++i)
    {
        dlib::image_window::overlay_line l(face_origin.part(i), face_dest.part(i), color);
        lines.push_back(l);
    }
    return lines;
}

void FaceDetector::toggle_camera_image() {
    if (show_camera_image) {
        win.set_image(dlib::array2d<dlib::rgb_pixel>(0, 0));
        win.set_size(width, height);
    }
    show_camera_image = !show_camera_image;
}

void FaceDetector::capture_neutral_face() { neutral_face_flag = CAPTURE_NEUTRAL_FACE; }

void FaceDetector::show_neutral_deltas() { neutral_face_flag = SHOW_NEUTRAL_DELTAS; }

void FaceDetector::reset_neutral_face() { neutral_face_flag = RESET_NEUTRAL_FACE; }

int FaceDetector::start(Writer &writer) {
    
    cv::VideoCapture camera(camera_number);
    if (!camera.isOpened())
    {
        std::cerr << "Writer is unable to connect to camera " << camera_number
                << std::endl;
        return -1;
    }
    
    if (!fps) fps = camera.get(CV_CAP_PROP_FPS); 
    camera.set(CV_CAP_PROP_FPS, fps);    
    
    if (!width) width = camera.get(CV_CAP_PROP_FRAME_WIDTH);
    camera.set(CV_CAP_PROP_FRAME_WIDTH, width * (1.0 / downsample_ratio));
    
    if (!height) height = camera.get(CV_CAP_PROP_FRAME_HEIGHT);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT, height * (1.0 / downsample_ratio));
        
    std::cout << "Camera output: " << camera.get(CV_CAP_PROP_FRAME_WIDTH) 
            << "x" << camera.get(CV_CAP_PROP_FRAME_HEIGHT) 
            << std::endl;
    
    std::cout << "Camera max fps: " << camera.get(CV_CAP_PROP_FPS) << std::endl;
    
    std::cout << "Initializing face detection..." << std::endl;
    
    /* dlib face detection variables */
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    dlib::deserialize(face_landmarks_location) >> sp;
    std::vector<dlib::rectangle> faces;
    dlib::full_object_detection shape, neutral_shape;
    std::vector<dlib::full_object_detection> shapes;
    dlib::rectangle main_face;
    int face_not_found = 0;

    
    /* dlib graphical window context */
    win.set_size(width, height);
    win.set_background_color(0, 0, 0);
    //dlib::button test(win);
    dlib::menu_bar mbar(win);
    mbar.set_number_of_menus(2);
    mbar.set_menu_name(0,"Settings",'S');
    mbar.set_menu_name(1, "Neutral Face", 'N');
    
    mbar.menu(0).add_menu_item(dlib::menu_item_text("Show/Hide Camera Image",*this,&FaceDetector::toggle_camera_image,'T'));
    
    mbar.menu(1).add_menu_item(dlib::menu_item_text("Capture Neutral Face",*this,&FaceDetector::capture_neutral_face,'C'));
    mbar.menu(1).add_menu_item(dlib::menu_item_text("Show Deltas",*this,&FaceDetector::show_neutral_deltas,'D'));
    mbar.menu(1).add_menu_item(dlib::menu_item_text("Clear",*this,&FaceDetector::reset_neutral_face,'C'));


    //test.set_pos(10,300);
    //test.set_name("button");
    win.set_title("Stringless");
    
    
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
        cv::Mat denoise_frame;
        cv::fastNlMeansDenoisingColored(frame, denoise_frame, 10, 10, 7, 5);
        dlib::gaussian_blur(c, cimg, 2);
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
 
        // Set largest face to be the main/selected face
        for (dlib::rectangle r : faces) {
            if (!main_face.is_empty() && r.area() > main_face.area()) {
                main_face = r;
            }
        }

        if (face_not_found < 5) {
            
            if (landmark_sample_per_frame > 1) {
                std::vector<dlib::point> avg;
                shapes.clear();
                for (int i = 0; i < landmark_sample_per_frame; ++i) {
                    shapes.push_back(sp(cimg, main_face));
                }
            
                for (int i = 0; i < points; ++i) {
                    long avg_x = 0, avg_y = 0;
                    for (dlib::full_object_detection s : shapes) {
                        avg_x += s.part(i).x();
                        avg_y += s.part(i).y();
                    }
                    
                    avg_x /= shapes.size();
                    avg_y /= shapes.size();
                    avg.push_back( dlib::point(avg_x, avg_y) );
                }
                
                shape = dlib::full_object_detection(main_face, avg);
                
            } else {
                // Find the pose of face
                shape = sp(cimg, main_face);
            }
            
            if (neutral_face_flag == CAPTURE_NEUTRAL_FACE)
                neutral_shape = shape;
            
            // Push data points into frame data
            for(int i = 0; i < points; ++i) {
                frame_data.points[i].x = (double)shape.part(i).x() / (double)cimg.nr();
                frame_data.points[i].y = (double)shape.part(i).y() / (double)cimg.nc();
            }
            
        } else {
            shape = dlib::full_object_detection();
        }
        frame_data.fps = frames_per_second;
        frame_data.frame_number = frame_count;
        
        
        std::cout << " Frame rate: " << frames_per_second << " Frame count: " 
                << frame_count << "      \r";
        ++frame_count;
        
        win.clear_overlay();
        if (show_camera_image) {
            win.set_image(cimg);
        }
        win.add_overlay(render_faces(shape));
        if (neutral_face_flag >= SHOW_NEUTRAL_FACE) 
            win.add_overlay(render_faces(neutral_shape, dlib::rgb_pixel(255,0,0)));
        else neutral_shape = dlib::full_object_detection(); // Reset neutral face
        if (neutral_face_flag == SHOW_NEUTRAL_DELTAS) {
            // Don't render deltas if no current face is found
            if (shape.num_parts() > 0) win.add_overlay(render_deltas(neutral_shape, shape));
        }

        
        if (neutral_face_flag == CAPTURE_NEUTRAL_FACE)
            neutral_face_flag = SHOW_NEUTRAL_FACE; // Neutral face is captured, now only display
        
        // Pass frame data to writer
        writer.write(frame_data);

    }
    
    camera.release();
    return 0;
}

} // namespace Stringless