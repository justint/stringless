/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   MemoryManager.cc
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 * 
 * Created on February 2, 2017, 11:30 AM
 */

#include "MemoryManager.h"
#include "FrameData.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>
#include <new> /* For O_* constants */

#include <iostream>

namespace Stringless {

/*
 * Constructs a Memory Manager that handles the shared memory construction,
 * passing of memory address, and deletion.
 */
MemoryManager::MemoryManager(std::string shared_memory_name, 
                             size_t shared_memory_size) 
{
    strcpy(this->shared_memory_name, shared_memory_name.c_str());
    this->shared_memory_size = shared_memory_size;
}

MemoryManager::MemoryManager(const MemoryManager& orig) {
    file_descriptor = orig.file_descriptor;
    *shared_memory_name = *orig.shared_memory_name;
    shared_memory_size = orig.shared_memory_size;
}

MemoryManager::~MemoryManager() {
    remove();
}

/*
 * Initializes the shared memory. 
 * 
 * If successful, returns zero, otherwise returns a non-zero number (errno) 
 * which specifies the exact type of error.
 */
int MemoryManager::init() { 
    // Create shared memory with read/write permissions
    // See Linux man page for open (https://linux.die.net/man/2/open) for mode
    // symbolic constant definitions
    file_descriptor = shm_open(shared_memory_name, O_CREAT | O_RDWR, S_IRWXU);
    
    if (file_descriptor == -1) // Memory opening error
    {
        std::cout << "MemoryManager " << this->shared_memory_name << 
            " failed to allocate shared memory with error: " << 
            strerror(errno) << std::endl;
        
        return errno;
    }
    
    int err;
    // Size the shared memory
    err = ftruncate(file_descriptor, shared_memory_size);
    if (err) { 
        std::cout << "MemoryManager " << this->shared_memory_name << 
            " failed to truncate shared memory with error: " << 
            strerror(errno) << std::endl;
    }
    return err;
}

/*
 * Returns a pointer to the shared memory, if it exists.
 */
FrameData * MemoryManager::address() {
    FrameData *frame_data;
    
    frame_data = (FrameData *)mmap(NULL, 
                                   shared_memory_size, 
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED,
                                   file_descriptor,
                                   0);
    
    if (frame_data == (void *) -1) {
        std::cout << "MemoryManager " << this->shared_memory_name << 
            " failed to acquire address for shared memory " << 
            "with error: " << strerror(errno) << std::endl;
    }
    return frame_data;
}

int MemoryManager::remove() {
    file_descriptor = shm_unlink(shared_memory_name);
    if (file_descriptor == -1) {
        std::cout << "MemoryManager " << this->shared_memory_name << 
            " failed to remove shared memory" << std::endl;
    } else {
        std::cout << "MemoryManager " << this->shared_memory_name << 
            " successfully removed shared memory" << std::endl;
    }
    return file_descriptor;
}

} // namespace Stringless
