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
#include "SafeBuffer.h"
#include "Histogram.h"
using namespace std;

struct thread_data {
    int n;
    string name;
    SafeBuffer* request_buffer;
};

struct worker_data {
    Histogram* hist;
    SafeBuffer* request_buffer;
    RequestChannel* workerChannel;
};


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
			delete my_data->workerChannel;
            break;
        }else{
			string response = my_data->workerChannel->cread();
            //cout << "adding to histogram " << request << ", " << response << endl;
			my_data->hist->update (request, response);
		}
    }
    pthread_exit(NULL);
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 100; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:w:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg); //This won't do a whole lot until you fill in the worker thread function
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

        cout << "CLIENT STARTED:" << endl;
        cout << "Establishing control channel... " << flush;
        RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
        cout << "done." << endl<< flush;

		SafeBuffer request_buffer;
		Histogram hist;
        
        /*
         * Sequential
        for(int i = 0; i < n; ++i) {
            request_buffer.push("data John Smith");
            request_buffer.push("data Jane Smith");
            request_buffer.push("data Joe Smith");
        }
        */
        pthread_t threads[3];
        pthread_t worker[w];
        struct thread_data td[3];
        struct worker_data wd[w];
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
        // Join them all back
        for(int i = 0; i < 3; i++){
            pthread_join(threads[i], NULL);
        }

        cout << "Done populating request buffer" << endl;

        cout << "Pushing quit requests... ";
        for(int i = 0; i < w; ++i) {
            request_buffer.push("quit");
        }
        cout << "done." << endl;
  
        
        //struct timeval tp_start, tp_end; /* Used to compute elapsed time. */
        //gettimeofday(&tp_start, 0); // start timer
        string s;

        // Spawn the worker threads
        for(int i = 0; i < w; ++i){
            wd[i].hist = &hist;
            wd[i].request_buffer = &request_buffer;
            chan->cwrite("newchannel");
		    s = chan->cread();
            cout << "Response: " << s << endl;
            // TODO check this
            wd[i].workerChannel = new RequestChannel(s, RequestChannel::CLIENT_SIDE);
            pthread_create(&worker[i], NULL, worker_thread_function, (void *) &wd[i]);
        }
        for(int i = 0; i < w; ++i){
            pthread_join(worker[i], NULL);
        }

        /*
         * Sequential
        while(true) {
            string request = request_buffer.pop();
			workerChannel->cwrite(request);

			if(request == "quit") {
			   	delete workerChannel;
                break;
            }else{
				string response = workerChannel->cread();
				hist.update (request, response);
			}
        }
        */
        //gettimeofday(&tp_end, 0);   // stop timer
        chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl; 

		hist.print ();
    }
}
