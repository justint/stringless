/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   MemoryManager.h
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 *
 * Created on February 2, 2017, 11:30 AM
 */

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <sys/types.h>

#include <string>

#include "FrameData.h"

namespace Stringless {

class MemoryManager {
public:
    MemoryManager(const std::string shared_memory_name, 
                  const size_t shared_memory_size);
    MemoryManager(const MemoryManager& orig);
    virtual ~MemoryManager();
    
    int init();
    FrameData *address();
    int remove();
    
private:
    int file_descriptor;
    off_t shared_memory_size;
    char shared_memory_name[1024];
    
};

} // namespace Stringless

#endif /* MEMORYMANAGER_H */

