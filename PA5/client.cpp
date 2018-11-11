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
#include <map>
#include <iterator>

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

struct statistics_data {
    Histogram* hist;
    string request;
    BoundedBuffer* response_buffer;
    int n;
};

struct event_data {
    int w;
    int n;
    BoundedBuffer* request_buffer;
    BoundedBuffer* response_buffer1;
    BoundedBuffer* response_buffer2;
    BoundedBuffer* response_buffer3;
    RequestChannel* control;
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

void* event_handler_thread_function(void *arg) {

    struct event_data *my_data;
    my_data = (struct event_data *) arg;
    string s;
    vector<RequestChannel*> wd;
    vector<int> fd_r;
    vector<int> fd_ready;
    vector<int> fd_wait;
    map<int,RequestChannel*> fd_map;
    map<RequestChannel*, string> request_map;
    fd_set fds;
    fd_set master;
    int requests = 3 * my_data->n;
    int counter = 0;
    // Create the request channels
    for(int i = 0; i < my_data->w; ++i){
            my_data->control->cwrite("newchannel");
		    s = my_data->control->cread();
            RequestChannel* current = new RequestChannel(s, RequestChannel::CLIENT_SIDE);
            wd.push_back(current);
            // Get read fds
            fd_r.push_back(current->read_fd());
            // update map with fd and request channel
            fd_map.insert(pair <int, RequestChannel*> (current->read_fd(), current));
            // Send an initial request
            string request = my_data->request_buffer->pop();
            current->cwrite(request);
            request_map.insert(pair <RequestChannel*, string> (current, request));
    }
    FD_ZERO(&master);
    for(int i = 0; i < fd_r.size(); i++){
        FD_SET(fd_r[i], &master);
    }
    while(counter < requests){
        // Zero out the fd set
        FD_ZERO(&fds);
        // Fill up the fd set again
        fds = master;
        int max_fd = fd_r.at(fd_r.size() - 1) + 1;
        if(select(max_fd, &fds, NULL, NULL, NULL) > 0){
            // we have a channel ready
            map<int, RequestChannel*>::iterator it;
            map<RequestChannel*, string>::iterator it_request;
            // find the free request channel
            for(int i = 0; i < fd_r.size(); i++){
                if(FD_ISSET(fd_r[i],&fds)){
                    // add it to a vector of fds
                    fd_ready.push_back(fd_r[i]);
                }
            }
            for(int i = 0; i < fd_ready.size(); i++){
                it = fd_map.find(fd_ready[i]);
                string response = it->second->cread();
                // check the request map for the old request
                it_request = request_map.find(it->second);
                string old_request = it_request->second;
                // Push the appropriate responses to their corresponding buffers
                if(old_request == "data John Smith"){
                    my_data->response_buffer1->push(response);
                }
                else if(old_request == "data Jane Smith"){
                    my_data->response_buffer2->push(response);
                }
                else if(old_request == "data Joe Smith"){
                     my_data->response_buffer3->push(response);
                }
                // Done with read so send new request
                request_map.erase(it_request);
                counter++;
                string request = my_data->request_buffer->pop();
                it->second->cwrite(request);
                // update map
                request_map.insert(pair<RequestChannel*, string> (it->second, request));
                cout << "Counter: " << counter << endl;
                cout << "Requests: " << requests << endl;
            }
            fd_ready.erase(fd_ready.begin(), fd_ready.end());
        } else {
            // we don't have a channel ready
        }
    }
    /*for(int i = 0; i < wd.size(); i++){
        delete[] wd[i];
    }*/
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
        pthread_t statistics[3];
        pthread_t event;
        struct thread_data td[3];
        struct statistics_data sd[3];
        struct event_data ed;

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
       
        // Initialize the event handler
        ed.control = chan;
        ed.n = n;
        ed.w = w;
        ed.request_buffer = &request_buffer;
        ed.response_buffer1 = buffer1;
        ed.response_buffer2 = buffer2;
        ed.response_buffer3 = buffer3;
        pthread_create(&event, NULL, event_handler_thread_function, (void *) &ed);

        // Join all the threads back
        for(int i = 0; i < 3; ++i){
            pthread_join(threads[i], NULL);
        }
        for(int i = 0; i < 3; ++i){
            pthread_join(statistics[i], NULL);
        }
        pthread_join(event, NULL);
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
