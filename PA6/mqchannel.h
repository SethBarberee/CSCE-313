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
        MQRequestChannel(const string _name, const Side _side, key_t control_key, int control_msg);
        ~MQRequestChannel();
        string cread();
        void cwrite(string _msg);
        key_t get_key();
        int get_msgid();
        int get_send();
        int get_receive();
};
