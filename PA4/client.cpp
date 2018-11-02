/*
    Based on original assignment by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31
 */


#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

#include <sys/time.h>
#include <cassert>
#include <assert.h>

#include <cmath>
#include <numeric>
#include <algorithm>

#include <list>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "reqchannel.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include <chrono>
#include <csignal>
using namespace std;

// Global variables
Histogram hist;

// Storage structs
struct thread_data {
    int n;
    string name;
    BoundedBuffer* request_buffer;
};

struct worker_data {
    BoundedBuffer* request_buffer;
    BoundedBuffer* response_buffer1;
    BoundedBuffer* response_buffer2;
    BoundedBuffer* response_buffer3;
    RequestChannel* workerChannel;
};

struct statistics_data {
    Histogram* hist;
    string request;
    BoundedBuffer* response_buffer;
    int n;
};

// Thread functions
void* request_thread_function(void *arg) {
	/*
		Fill in this function.

		The loop body should require only a single line of code.
		The loop conditions should be somewhat intuitive.

		In both thread functions, the arg parameter
		will be used to pass parameters to the function.
		One of the parameters for the request thread
		function MUST be the name of the "patient" for whom
		the data requests are being pushed: you MAY NOT
		create 3 copies of this function, one for each "patient".
	 */
    struct thread_data *my_data;
    my_data = (struct thread_data *) arg;
    string data = "data " + my_data->name;
    // Push n requests to the buffer
	for(int i = 0; i < my_data->n; i++) {
        my_data->request_buffer->push(data);        
	}
    pthread_exit(NULL);
}

void* worker_thread_function(void* arg) {
    /*
		Fill in this function. 

		Make sure it terminates only when, and not before,
		all the requests have been processed.

		Each thread must have its own dedicated
		RequestChannel. Make sure that if you
		construct a RequestChannel (or any object)
		using "new" that you "delete" it properly,
		and that you send a "quit" request for every
		RequestChannel you construct regardless of
		whether you used "new" for it.
     */
    struct worker_data *my_data;
    my_data = (struct worker_data *) arg;

    while(true) {
        string request = my_data->request_buffer->pop();
		my_data->workerChannel->cwrite(request);

		if(request == "quit") {
            // quitting so delete the worker channel
			delete my_data->workerChannel;
            break;
        }else{
			string response = my_data->workerChannel->cread();
            // Push the appropriate responses to their corresponding buffers
            if(request == "data John Smith"){
                my_data->response_buffer1->push(response);
            }
            else if(request == "data Jane Smith"){
                my_data->response_buffer2->push(response);
            }
            else if(request == "data Joe Smith"){
                my_data->response_buffer3->push(response);
            }
		}
    }
    pthread_exit(NULL);
}

void* statistics_thread_function(void* arg) {
    /*
		Fill in this function. 

		Make sure it terminates only when, and not before,
		all the requests have been processed.

		Each thread must have its own dedicated
		RequestChannel. Make sure that if you
		construct a RequestChannel (or any object)
		using "new" that you "delete" it properly,
		and that you send a "quit" request for every
		RequestChannel you construct regardless of
		whether you used "new" for it.
     */
    struct statistics_data *my_data;
    string response = "";
    my_data = (struct statistics_data *) arg;
    // Take a response and push it to the histogram
    for(int i = 0; i < my_data->n; i++){
        response = my_data->response_buffer->pop();
	    my_data->hist->update (my_data->request, response);
    }
    pthread_exit(NULL);
}

void histogram_update(int signal){
    // This function is called on every SIGALRM signal which
    // occurs every 2 seconds
    //
    // This will clear the screen and print the histogram
    system("clear");
    hist.print();
    // reregister the alarm
    alarm(2);
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 100; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int b = n * 3; // default capacity for BoundedBuffer
    int opt = 0;
    // register the custom signal handler for SIGALRM
    signal(SIGALRM, histogram_update);
    alarm(2);
    // Commandline args parsing
    while ((opt = getopt(argc, argv, "n:w:b:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
			}
    }

    int pid = fork();
	if (pid == 0){
		execl("dataserver", (char*) NULL);
	}
	else {

        cout << "n == " << n << endl;
        cout << "w == " << w << endl;
        cout << "b == " << b << endl;

        cout << "CLIENT STARTED:" << endl;
        cout << "Establishing control channel... " << flush;
        RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
        cout << "done." << endl<< flush;

		BoundedBuffer request_buffer(b);
        BoundedBuffer* buffer1;
        BoundedBuffer* buffer2;
        BoundedBuffer* buffer3;
        if(b > 2){
            buffer1 = new BoundedBuffer((b/3));
            buffer2 = new BoundedBuffer((b/3));
            buffer3 = new BoundedBuffer((b/3));
        }
        else {
            buffer1 = new BoundedBuffer(1);
            buffer2 = new BoundedBuffer(1);
            buffer3 = new BoundedBuffer(1);
        }

        pthread_t threads[3];
        pthread_t worker[w];
        pthread_t statistics[3];
        struct thread_data td[3];
        struct worker_data wd[w];
        struct statistics_data sd[3];

        td[0].n = n;
        td[0].name = "John Smith";
        td[0].request_buffer = &request_buffer;
        td[1].n = n;
        td[1].name = "Jane Smith";
        td[1].request_buffer = &request_buffer;
        td[2].n = n;
        td[2].name = "Joe Smith";
        td[2].request_buffer = &request_buffer;
        // Spawn a thread for each patient
        for(int i = 0; i < 3; i++){
            pthread_create(&threads[i], NULL, request_thread_function, (void *) &td[i]);
        }
        auto start = chrono::system_clock::now(); 
        string s; 
        // Spawn the worker threads
        for(int i = 0; i < w; ++i){
            wd[i].request_buffer = &request_buffer;
            wd[i].response_buffer1 = buffer1;
            wd[i].response_buffer2 = buffer2;
            wd[i].response_buffer3 = buffer3;
            chan->cwrite("newchannel");
		    s = chan->cread();
            wd[i].workerChannel = new RequestChannel(s, RequestChannel::CLIENT_SIDE);
            pthread_create(&worker[i], NULL, worker_thread_function, (void *) &wd[i]);
        }

        // Initialize all the values for statistics threads
        sd[0].response_buffer = buffer1;
        sd[0].request = "data John Smith";
        sd[0].hist = &hist;
        sd[0].n = n;
        sd[1].response_buffer = buffer2;
        sd[1].request = "data Jane Smith";
        sd[1].hist = &hist;
        sd[1].n = n;
        sd[2].response_buffer = buffer3;
        sd[2].request = "data Joe Smith";
        sd[2].hist = &hist;
        sd[2].n = n;
        // Initialize the stats threads
        for(int i = 0; i < 3; i++){
            pthread_create(&statistics[i], NULL, statistics_thread_function, (void *) &sd[i]); 
        }

        // Join all the threads back
        for(int i = 0; i < 3; ++i){
            pthread_join(threads[i], NULL);
        }
        for(int i = 0; i < w; ++i) {
            request_buffer.push("quit");
        }
        for(int i = 0; i < w; ++i){
            pthread_join(worker[i], NULL);
        }
        for(int i = 0; i < 3; ++i){
            pthread_join(statistics[i], NULL);
        }
        auto end = chrono::system_clock::now();
        chrono::duration<double> elaspsed_seconds = end-start;
        chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl;
        // Clear the screen and print the histogram (to keep it with the rest of the histograms)
        system("clear");
		hist.print ();
        cout << "Time: " << elaspsed_seconds.count() << " secs" << endl;
    }
}
