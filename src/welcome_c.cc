// cs180_shared_memory_test
// 
// Practice with shared memory in linux environment
// Run with arg '1' (without quotes) to remove shared memory, 2 to read it

#include <iostream>
#include <string>
#include <sstream>

#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "welcome.h"

int main(int argc, char**argv) {
    
    const char* shm_name = "/stringless";
    
    const int shm_length = 100;
    
    const int region_length = 1000;
    struct region {
        int len;
        char buf[region_length];
    };
    
    
    if (argc > 1){
        if (atoi(argv[1]) == 1) // Deletion of shared memory
        {
            int fd;
            fd = shm_unlink(shm_name);
            if (fd == -1) {
                std::cout << "Failed shared memory deletion. Has it already" <<
                        " been deleted?" << std::endl;
                return 1;
            } else {
                std::cout << "Successful shared memory deletion" << std::endl;
            }
        }
        else if (atoi(argv[1]) == 2) // Reading shared memory
        {
            ////   TESTING SYNC
            while(true)
            {
                int fd;
                fd = shm_open(shm_name, O_RDONLY, 0777);
                if (fd == -1) {
                    std::cout << "Failed to read shared memory: " << strerror(errno) << std::endl;
                    return 1;
                } else {
                    region *addr = 
                        (region *)mmap(NULL, sizeof(struct region), PROT_READ, MAP_SHARED, fd, 0);
                    if (addr == (void *) -1) {
                        std::cout << "mmap read access failure" << std::endl;
                    }

                    int err;
                    pthread_mutex_t shm_mutex;
                    pthread_mutexattr_t attr;

                    // Initiate mutex attr
                    err = pthread_mutexattr_init(&attr);
                    if (err) return err;

                    // Declare mutex as shared
                    err = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
                    if (err) return err;

                    // Initiate mutex
                    err = pthread_mutex_init(&shm_mutex, &attr);
                    if (err) return err;


                    // Locking mutex
                    err = pthread_mutex_lock(&shm_mutex);
                    if (err) return err;

                    std::cout << addr->buf << std::endl;

                    err = pthread_mutex_unlock(&shm_mutex);
                    if (err) return err;
                    
                    sleep(1);
                } 
            }
        }
    }
    else {
        int err;
        pthread_mutex_t shm_mutex;
        pthread_mutexattr_t attr;
        
        // Initiate mutex attr
        err = pthread_mutexattr_init(&attr);
        if (err) return err;
        
        // Declare mutex as shared
        err = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        if (err) return err;
        
        // Initiate mutex
        err = pthread_mutex_init(&shm_mutex, &attr);
        if (err) return err;
        
        int fd;        
        fd = shm_open(shm_name, O_CREAT | O_RDWR, 0777);
        if (fd == -1) { // Memory opening error
            std::cout << "Shared memory open failure: " << fd << std::endl;
            return 1;
        } else {
            std::cout << "Successful shared memory creation" << std::endl;
        }
        

        // Size the shared memory
        ftruncate(fd, sizeof(struct region));
        
        region *addr =
            (region *)mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == (void *) -1) {
            std::cout << "mmap write access failure" << std::endl;
        }
 
        
        ////  TESTING SYNC BELOW
        
        int i = 0;
        std::ostringstream oss;
        while(true)
        {        
            oss.str("");
            oss << "Hello world! " << i;
            
            // Lock mutex while writing to memory
            err = pthread_mutex_lock(&shm_mutex);
            if (err) return err;

            strcpy(addr->buf, oss.str().c_str());

            err = pthread_mutex_unlock(&shm_mutex);
            if (err) return err;
            
            
            sleep(1);
            ++i;
        }
        
        
    }
    
    return 0;
}
