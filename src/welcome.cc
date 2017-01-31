// cs180_shared_memory_test
// 
// Practice with shared memory in linux environment, using Boost
// Run with arg '1' (without quotes) to remove shared memory, 2 to read it

#include <iostream>
#include <mutex>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include <unistd.h>

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
                std::cout << static_cast<int>(*mem++) << std::endl;
            }
        }
    }
    else {
        const offset_t memory_size = 2048;

        managed_shared_memory managed_shm(open_or_create, "shared_memory", memory_size);
        
        //Allocate 100 bytes of memory from segment, throwing version
         void *ptr = managed_shm.allocate(10);
         
         // Create the named mutex
         named_mutex mutex(open_or_create, "shared_memory_mutex");
         
         std::mutex mtx;
         
         
            // Lock the mutex
            //scoped_lock<named_mutex> lock(mutex);
            //mutex.lock();
            //std::memset(managed_shm.get_address(), 1, managed_shm.get_size());
            mtx.lock();
            std::cout << "Hello" << std::endl;
            mtx.unlock();
            //sleep(10);
            //mutex.unlock();
         
         
         
         //Deallocate it
         //managed_shm.deallocate(ptr);

        
        /*
        shared_memory_object shm_obj
                    (open_or_create,
                    "shared_memory", 
                    read_write);
        
        shm_obj.truncate(truncate_length);    

        std::cout << "Successfully truncated shared memory to size " << 
                truncate_length << std::endl;

        // Create mapped region of whole shared memory
        mapped_region region(shm_obj, read_write);

        std::cout << region.get_address() << std::endl;
        std::cout << region.get_size() << std::endl;
        
        */
        
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
        
        
        // Mutex test - will write for some time while I try to read from
        // another process.
        /*
        std::cout << "Beginning mutex lock..." << std::endl;
        named_mtx.lock();
            std::memset(region.get_address(), 1, region.get_size());
        named_mtx.unlock();
        std::cout << "Mutex unlocked" << std::endl;
         */
    }
    
    return 0;
}
