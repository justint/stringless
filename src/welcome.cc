// cs180_shared_memory_test
// 
// Practice with shared memory in linux environment, using Boost
// Run with arg '1' (without quotes) to remove shared memory, 2 to read it

#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "welcome.h"

int main(int argc, char**argv) {
    
    using namespace boost::interprocess;

    if (argc > 1){
        if (atoi(argv[1]) == 1) // Deletion of shared memory
        {
            if (shared_memory_object::remove("shared_memory"))
                std::cout << "Successfully removed shared memory" << std::endl;
            else std::cout << "Shared memory already deleted or does not exist" <<
                    std::endl;
        }
        else if (atoi(argv[1]) == 2) // Reading shared memory
        {
            shared_memory_object shm_obj (open_only, "shared_memory", read_only);
            mapped_region region(shm_obj, read_only);

            char *mem = static_cast<char*>(region.get_address());
            for (auto i = 0; i < region.get_size(); ++i)
            {
                std::cout << *mem++ << std::endl;
            }
        }
    }
    else {
        const offset_t truncate_length = 10;

        shared_memory_object shm_obj
                    (create_only,
                    "shared_memory", 
                    read_write);

        shm_obj.truncate(truncate_length);    

        std::cout << "Successfully truncated shared memory to size " << 
                truncate_length << std::endl;

        size_t shm_size = sizeof(truncate_length);

        // Create mapped region of whole shared memory
        mapped_region region(shm_obj, read_write);

        std::cout << region.get_address() << std::endl;
        std::cout << region.get_size() << std::endl;

        // Set all memory to 1 (works!)
        //std::memset(region.get_address(), 1, region.get_size());
        
        // Incremental writing test (works!)
        /*
        int i;
        for (i = 0; i < region.get_size(); ++i)
        {
            std::memset(region.get_address() + i, i, 1);
        }
        */
        
        // String writing test (works!)
        /*
        char test[] = "Sample frame data";
        std::memcpy(region.get_address(), test, region.get_size());
         */
        
    }
    
    return 0;
}
