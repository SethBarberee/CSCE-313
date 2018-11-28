#include "reqchannel.h"
#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <mqueue.h>
#include <cstring>

class MQRequestChannel : public RequestChannel {
    private:
        key_t key;
        int msgid;
        int send_id;
        int receive_id;
    public:
        MQRequestChannel(const string _name, const Side _side);
        ~MQRequestChannel();
        string cread();
        void cwrite(string _msg);
        int get_send();
        int get_receive();
};
