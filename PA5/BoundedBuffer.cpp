#include "BoundedBuffer.h"
#include <string>
#include <queue>
using namespace std;

BoundedBuffer::BoundedBuffer(int _cap) {
	pthread_mutex_init(&m, NULL);
    pthread_mutex_init(&pop_mtx, NULL);
    pthread_mutex_init(&push_mtx, NULL);
    pthread_cond_init(&overflow,NULL);
    pthread_cond_init(&underflow, NULL);
    push_certs = _cap;
    pop_certs = 0;
    // Use capactity for the buffer
    buffer_size = _cap;
}

BoundedBuffer::~BoundedBuffer() {
    pthread_mutex_destroy(&m);
    pthread_mutex_destroy(&pop_mtx);
    pthread_mutex_destroy(&push_mtx);
    pthread_cond_destroy(&overflow);
    pthread_cond_destroy(&underflow);
}

int BoundedBuffer::size() {
    pthread_mutex_lock(&m);
	int size = q.size();
    pthread_mutex_unlock(&m);
    return size;
}

void BoundedBuffer::push(string str) {
	/*
	Is this function thread-safe??? Does this automatically wait for the pop() to make room 
	when the buffer if full to capacity???
	*/
    pthread_mutex_lock(&push_mtx);
    push_certs--;

    if(push_certs < 0){
        pthread_cond_wait(&overflow, &push_mtx); 
    }
    pthread_mutex_unlock(&push_mtx);
    pthread_mutex_lock(&m);
	q.push (str);
    pthread_mutex_unlock(&m);
    pthread_mutex_lock(&pop_mtx);
    pop_certs++;
    // send underflow if needed
    if(pop_certs <= 0){
        pthread_cond_signal(&underflow);
    }
    pthread_mutex_unlock(&pop_mtx);
}

string BoundedBuffer::pop() {
	/*
	Is this function thread-safe??? Does this automatically wait for the push() to make data available???
	*/
    pthread_mutex_lock(&pop_mtx);
    pop_certs--;
    // this predicate is needed or program will deadlock since there can be spurious wake-ups (Lecture 11)
    if(pop_certs < 0){
        pthread_cond_wait(&underflow, &pop_mtx); 
    }
    pthread_mutex_unlock(&pop_mtx);
    pthread_mutex_lock(&m);
	string s = q.front();
	q.pop();
    pthread_mutex_unlock(&m);
    pthread_mutex_lock(&push_mtx);
    push_certs++;
    // send overflow signal
    if(push_certs <= 0){
        pthread_cond_signal(&overflow);
    }
    pthread_mutex_unlock(&push_mtx);
	return s;
}
