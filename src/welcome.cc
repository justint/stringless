// cs180_shared_memory_test
// 
// Practice with shared memory in linux environment, using Boost
// Run with arg '1' (without quotes) to remove shared memory

#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "welcome.h"

int main(int argc, char**argv) {
    
    using namespace boost::interprocess;

    if (atoi(argv[1]) == 1)
    {
        shared_memory_object::remove("shared_memory");
        std::cout << "Successfully removed shared memory" << std::endl;
    }
    else {
        const offset_t truncate_length = 100;

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

        // Write all memory to 1(?)
        std::memset(region.get_address(), 1234, region.get_size());
    }
    
    return 0;
}
