/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   Stringless.cc
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 *
 * Created on February 2, 2017, 11:28 AM
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "dlib/gui_widgets.h"

#include "FrameData.h"
#include "MemoryManager.h"
#include "Writer.h"
#include "FaceDetector.h"

    
int main(int argc, char** argv) {
    
    std::vector<std::string> arg_list = {"-i", "-iw", "-r", "-h", "-flp"};

    std::string help_message = "Usage: stringless [-options]\n\nwhere options include:";
    
    if (argc == 0 || arg_list.at(3).compare(argv[1]) == 0 )
    {
        std::cout << help_message << std::endl;
        return 0;
    }
    
    
    ////// /* BEGIN NON-HELP ARGUMENT HANDING */ //////
    
    // Grab the face landmarks file location from args
    char* face_landmarks_location;
    for (int i = 0; i < argc; ++i)
    {
        if (arg_list.at(4).compare(argv[i]) == 0) { // -flp <flp location>
            face_landmarks_location = argv[i + 1];
        }
    }
    
    
    const std::string shared_memory_name = "/stringless";
    // Set shared memory size to two frames
    const size_t shared_memory_size = sizeof(struct Stringless::FrameData) * 3;
    // Select camera 0 for frame capturing
    const int camera_number = 0;
    
    
    
    Stringless::MemoryManager memory_manager(shared_memory_name, 
                                             shared_memory_size,
                                             Stringless::MemoryManager::write);

    if (arg_list.at(0).compare(argv[1]) == 0 || 
            arg_list.at(1).compare(argv[1]) == 0) // If arg is -i or -iw
    {
        if (memory_manager.init()) { 
            // If shared memory allocation fails, MemoryManager already prints 
            // error, we just exit properly.
            return errno;
        }

        if (arg_list.at(1).compare(argv[1]) == 0) // If arg is -iw
        {
            Stringless::Writer writer(memory_manager.address());
            Stringless::FaceDetector face_detector(camera_number,
                                                   face_landmarks_location);
            
            face_detector.start(writer);
        }

    } else { // If arg is -r
        memory_manager.remove();
    }

    
    return 0;
}

