#include "mqchannel.h"

struct mesg_buffer { 
    long mesg_type; 
    char mesg_text[100]; 
} message; 


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MQRequestChannel

MQRequestChannel::MQRequestChannel(const std::string _name, const Side _side) : RequestChannel(_name, _side){
    // TODO get nchannels from _name
    // name = "data" + nchannels + _
    RequestChannel::type = 'q';
    if(_side == CLIENT_SIDE){
        send_id = 1;
        // TODO find a way to change this
        receive_id = 2;
    }
    else {
        // TODO find a way to change this
        send_id = 2;
        receive_id = 1;
    }
    string file_name = "mq_" + _name;
    creat(file_name.c_str(), 0666);
    key = ftok(file_name.c_str(), 1);
    msgid = msgget(key, 0666 | IPC_CREAT);
}

MQRequestChannel::~MQRequestChannel(){
    msgctl(msgid, IPC_RMID, NULL);
    string file_name  = "mq_" + my_name;
    remove(file_name.c_str());
}

int MQRequestChannel::get_receive(){
    return receive_id;
}

int MQRequestChannel::get_send(){
    return send_id;
}

key_t MQRequestChannel::get_key(){
    return key;
}

int MQRequestChannel::get_msgid(){
    return msgid;
}

string MQRequestChannel::cread(){
    struct mesg_buffer { 
        long mesg_type; 
        char mesg_text[100]; 
    } message_receive; 
    int receive = MQRequestChannel::get_receive();
    msgrcv(msgid, &message_receive, sizeof(message_receive.mesg_text), receive, 0);
    string msg;
    msg = message_receive.mesg_text;
    return msg; 
}

void MQRequestChannel::cwrite(string _msg){
    struct mesg_buffer { 
        long mesg_type; 
        char mesg_text[100]; 
    } message; 
    strncpy(message.mesg_text, _msg.c_str(), _msg.size()+1);
    int length = _msg.size() + 1;
    message.mesg_type = MQRequestChannel::get_send();
    if(msgsnd(msgid, &message, length, 0) == -1){
        perror("msgsnd failure");
    }
}
