#include "fifochannel.h"
#include "unistd.h"
FIFORequestChannel::FIFORequestChannel(const string _name, const Side _side) : RequestChannel(_name, _side){
	if (_side == SERVER_SIDE) {
		open_write_pipe(pipe_name(WRITE_MODE).c_str());
		open_read_pipe(pipe_name(READ_MODE).c_str());
	}
	else {
		open_read_pipe(pipe_name(READ_MODE).c_str());
		open_write_pipe(pipe_name(WRITE_MODE).c_str());
	}
    RequestChannel::type = 'f';
}

FIFORequestChannel::~FIFORequestChannel(){

   	close(wfd);
	close(rfd);
	//if (my_side == SERVER_SIDE) {
		remove(pipe_name(READ_MODE).c_str());
		remove(pipe_name(WRITE_MODE).c_str());
	//}
 
}

string FIFORequestChannel::cread(){
    return RequestChannel::cread();
};

void FIFORequestChannel::cwrite(string msg){
    return RequestChannel::cwrite(msg);
}
