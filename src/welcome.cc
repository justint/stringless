#include <iostream>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "welcome.h"

int main(int argc, char**argv) {
    
    boost::interprocess::shared_memory_object shm_obj
            (boost::interprocess::open_or_create, 
            "shared_memory", 
            boost::interprocess::read_only);
    
    std::cout << "Successfully created + opened shared memory" << std::endl;
    
    boost::interprocess::shared_memory_object::remove("shared_memory");
    
    std::cout << "Successfully removed shared memory" << std::endl;
    return 0;
}
