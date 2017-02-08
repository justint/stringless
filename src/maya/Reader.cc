/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Reader.cc
 * Author: jtennant
 * 
 * Created on February 2, 2017, 12:22 PM
 */

#include "Reader.h"

#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../MemoryManager.h"

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
    FrameData incoming_data[2];
    
    mutex.lock();
    incoming_data[0] = data_address[0];
    incoming_data[1] = data_address[1];
    mutex.unlock();
    
    return incoming_data;
}

} // namespace Stringless

int main (int argc, char** argv) {
    const std::string shared_memory_name = "/stringless";
    // Set shared memory size to two frames
    const size_t shared_memory_size = sizeof(struct Stringless::FrameData) * 3;
    
    Stringless::MemoryManager memory_manager(shared_memory_name,
                                             shared_memory_size,
                                             Stringless::MemoryManager::read);
    
    if (memory_manager.init()) {
        return errno;
    }
    
    Stringless::Reader reader(memory_manager.address());
    
    while(true)
    {
        const Stringless::FrameData *incoming_data = reader.read();
        
        std::cout << "Hello!" << std::endl;
        
        struct stat s;
        fstat(memory_manager.get_file_descriptor(), &s);
        
        std::cout << "shm size: " << s.st_size << std::endl;
        std::cout << &incoming_data << std::endl;
        
        return 0;
    }
    
    return 0;
}