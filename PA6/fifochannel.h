#include "reqchannel.h"

class FIFORequestChannel : public RequestChannel {

    public:
    FIFORequestChannel(const string _name, const Side _side); 
    ~FIFORequestChannel();
    string cread();
    void cwrite(string msg);
};
