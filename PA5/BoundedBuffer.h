#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <stdio.h>
#include <queue>
#include <string>
using namespace std;

class BoundedBuffer {
private:
	queue<string> q;
    int buffer_size;
    int push_certs;
    int pop_certs;
    pthread_mutex_t m;
    pthread_mutex_t push_mtx;
    pthread_mutex_t pop_mtx;
    // two condition variables
    pthread_cond_t overflow;
    pthread_cond_t underflow;

public:
    BoundedBuffer(int);
	~BoundedBuffer();
	int size();
    void push (string);
    string pop();
};

#endif /* BoundedBuffer_ */
