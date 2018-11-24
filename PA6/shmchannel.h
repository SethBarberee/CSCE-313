#include "reqchannel.h"
#include <stdio.h> 
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <sys/sem.h>

// Kernel Semaphore class
//
// Needed for SHMRequestChannel
class KernelSemaphore {
    private:
        int semid;
    public:
        KernelSemaphore(int _val, key_t key);
        ~KernelSemaphore();

        void P(); /* Acquire lock */
        void V(); /* Release lock */
};

class SHMBoundedBuffer {
    private:
        KernelSemaphore* full;
        KernelSemaphore* empty;
        int shmid;
        char* data;
        string my_name;
    public:
        SHMBoundedBuffer(string name);
        ~SHMBoundedBuffer();

        void push(string msg);
        string pop();
};


class SHMRequestChannel : public RequestChannel {
    private:
        // Use a bounded buffer
        SHMBoundedBuffer* server_buffer;
        SHMBoundedBuffer* client_buffer;
        string my_name;
    public:
        SHMRequestChannel(const string _name, const Side _side);
        ~SHMRequestChannel();
        string cread();
        void cwrite(string _msg);
};
