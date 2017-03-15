/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   Reader.cc
 * Author: jtennant
 * 
 * Created on February 2, 2017, 12:22 PM
 */

#include "Reader.h"

#include <chrono>
#include <iostream>
#include <thread>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "MemoryManager.h"

namespace Stringless {

Reader::Reader(FrameData *data_address) : data_address(data_address) {
    mutex.init();
}

Reader::Reader(const Reader& orig) {
    data_address = orig.data_address;
    mutex = orig.mutex;
}

Reader::~Reader() { }

FrameData const * const Reader::read() {
    FrameData *incoming_data;
    
    mutex.lock();
    incoming_data = data_address;
    mutex.unlock();
    
    return incoming_data;
}

} // namespace Stringless
/*
int main (int argc, char** argv) {
    const std::string shared_memory_name = "/stringless";
    // Set shared memory size to two frames
    const size_t shared_memory_size = sizeof(struct Stringless::FrameData) * 2;
    
    Stringless::MemoryManager memory_manager(shared_memory_name,
                                             shared_memory_size,
                                             Stringless::MemoryManager::read);
    
    if (memory_manager.init()) {
        return errno;
    }
    
    Stringless::Reader reader((Stringless::FrameData*)memory_manager.address());
        
    const Stringless::FrameData *incoming_data;
    
    // Frame count / frames per second variables 
    int frame_count = 0;
    int frame_count_enter = 0, frame_count_exit;
    int frames_per_second;
    
    auto start = std::chrono::steady_clock::now();

    while(true)
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
            
        incoming_data = reader.read();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / incoming_data[0].fps));
        
        std::cout << "fps: " << frames_per_second << " frame 0 point 0 x: " << incoming_data[0].points[0].x << " y: " << incoming_data[0].points[0].y << "      \r";
        ++frame_count;
    }
    
    return 0;
}
*/