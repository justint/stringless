/* 
 * File:   Stringless.cc
 * Author: jtennant
 *
 * Created on February 2, 2017, 11:28 AM
 */

#include "FrameData.h"
#include "MemoryManager.h"
#include "Writer.h"

#include <iostream>
#include <string>
#include <vector>

/*
 * 
 */
    
int main(int argc, char** argv) {
    
    std::vector<std::string> arg_list = {"-i", "-iw", "-r", "-h"};
    
    std::string help_message = "Usage: stringless [-options]\n\nwhere options include:";
    
    if (argc == 0 || arg_list.at(3).compare(argv[1]) == 0 )
    {
        std::cout << help_message << std::endl;
        return 0;
    }
    
    /* BEGIN NON-HELP ARGUMENT HANDING */
    
    const std::string shared_memory_name = "/stringless";
    // Set shared memory size to two frames
    const size_t shared_memory_size = sizeof(struct Stringless::FrameData) * 2;
    
    Stringless::MemoryManager memory_manager(shared_memory_name, 
                                             shared_memory_size);
    
    if (arg_list.at(0).compare(argv[1]) == 0 || 
            arg_list.at(1).compare(argv[1]) == 0) // If arg is -i or -iw
    {
        if (memory_manager.init()) { // If shared memory allocation fails:
            // MemoryManager already prints error, we just exit properly.
            return errno;
        }

        // Construct memory writer with address to memory
        Stringless::Writer writer(memory_manager.address());

    } else { // If arg is -r
        memory_manager.remove();
    }
    
    return 0;
}

