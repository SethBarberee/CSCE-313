#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "reqchannel.h"
#include "fifochannel.h"
#include "mqchannel.h"
#include "shmchannel.h"
#include <pthread.h>
using namespace std;


int nchannels = 0;
pthread_mutex_t newchannel_lock;
void* handle_process_loop (void* _channel);

void process_newchannel(RequestChannel* _channel) {
	nchannels ++;
	string new_channel_name = "data" + to_string(nchannels) + "_";
	_channel->cwrite(new_channel_name);
    RequestChannel* data_channel = nullptr;
    if(_channel->get_type() == 'f'){
        data_channel = new FIFORequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
    } else if(_channel->get_type() == 'q'){
        data_channel = new MQRequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
    } else if(_channel->get_type() == 's'){
        data_channel = new SHMRequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
    } else {
        data_channel = new FIFORequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);
    }
	pthread_t thread_id;
	if (pthread_create(& thread_id, NULL, handle_process_loop, data_channel) < 0 ) {
		EXITONERROR ("");
	}
	
}

void process_request(RequestChannel* _channel, string _request) {

	if (_request.compare(0, 5, "hello") == 0) {
		_channel->cwrite("hello to you too");
	}
	else if (_request.compare(0, 4, "data") == 0) {
		usleep(1000 + (rand() % 5000));
		_channel->cwrite(to_string(rand() % 100));	
	}
	else if (_request.compare(0, 10, "newchannel") == 0) {
		process_newchannel(_channel);
	}
	else {
		_channel->cwrite("unknown request");
	}
}

void* handle_process_loop (void* _channel) {
	RequestChannel* channel = (RequestChannel *) _channel;
	for(;;) {
		string request = channel->cread();
		if (request.compare("quit") == 0) {
			break;                  // break out of the loop;
		}
		process_request(channel, request);
	}
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/


int main(int argc, char * argv[]) {
    int opt = 0;
    string i = "f";
    // getopt the args
    while ((opt = getopt(argc, argv, "i:")) != -1) {
        switch (opt) {
            case 'i':
                i = strdup(optarg);
                break;
			}
    }
	newchannel_lock = PTHREAD_MUTEX_INITIALIZER;
    RequestChannel *control_channel = nullptr;
    // What channel do we need?
    if(i == "f"){
        control_channel = new FIFORequestChannel("control", RequestChannel::SERVER_SIDE);
    } else if(i == "q"){
        control_channel = new MQRequestChannel("control", RequestChannel::SERVER_SIDE);
    } else if(i == "s"){
        control_channel = new SHMRequestChannel("control", RequestChannel::SERVER_SIDE);
    } else {
        control_channel = new FIFORequestChannel("control", RequestChannel::SERVER_SIDE);
    }
	handle_process_loop (control_channel);	
}

