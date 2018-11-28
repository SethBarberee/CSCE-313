#include "shmchannel.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KernelSemaphore
KernelSemaphore::KernelSemaphore(int _val, key_t key){
    semid = semget(key, 1, IPC_CREAT | 0666);
    struct sembuf sb = {0, (short)_val, 0};
    // don't semop if _val is 0
    if(_val != 0){
        semop(semid, &sb, 1);
    }
}

KernelSemaphore::~KernelSemaphore(){
    semctl(semid, IPC_RMID, 0);
}

void KernelSemaphore::P(){
    // Try to decrement
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

void KernelSemaphore::V(){
    // increments value
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
SHMBoundedBuffer::SHMBoundedBuffer(string name){
    my_name = "shm_" + name;
    creat(my_name.c_str(), 0666);
    key_t key = ftok(my_name.c_str(), 0);
    key_t key_full = ftok(my_name.c_str(), 1);
    key_t key_empty = ftok(my_name.c_str(), 2);
    full = new KernelSemaphore(0, key_full);
    empty = new KernelSemaphore(1, key_empty);

    shmid = shmget(key, 1024, 0666|IPC_CREAT);
    data = (char*) shmat(shmid, 0, 0); 
}

SHMBoundedBuffer::~SHMBoundedBuffer(){
    delete full;
    delete empty;
    shmdt(data);
    shmctl(shmid, IPC_RMID, 0);
    remove(my_name.c_str());
}

void SHMBoundedBuffer::push(string msg){
    empty->P();
    strncpy(data, msg.c_str(), msg.size()+1);
    full->V();
}

string SHMBoundedBuffer::pop(){
    full->P();
    string data_string(data);
    empty->V();
    return data_string;
}


//////////////////////////////////////
// SHMRequestChannel

SHMRequestChannel::SHMRequestChannel(const std::string _name, const Side _side) : RequestChannel(_name, _side){
        string name = _name + "2";
        server_buffer = new SHMBoundedBuffer(_name);
        client_buffer = new SHMBoundedBuffer(name);
        RequestChannel::type = 's';
}

SHMRequestChannel::~SHMRequestChannel(){
    delete server_buffer;
    delete client_buffer;
}

string SHMRequestChannel::cread(){
    // pop from the buffer
    if(RequestChannel::my_side == SERVER_SIDE){
        return server_buffer->pop();
    }
    else {
        return client_buffer->pop();
    }
}

void SHMRequestChannel::cwrite(string _msg){
    // push to the buffer here
    if(RequestChannel::my_side == SERVER_SIDE){
        client_buffer->push(_msg);
    }
    else {
        server_buffer->push(_msg);
    }
}   
