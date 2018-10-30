# makefile

all: dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	g++ -g -w -Wall -O0 -std=c++11 -c reqchannel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -g -w -Wall -O0 -std=c++11 -c BoundedBuffer.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -Wall -O0 -std=c++11 -c Histogram.cpp


dataserver: dataserver.cpp reqchannel.o 
	g++ -g -w -Wall -O0 -std=c++11 -o dataserver dataserver.cpp reqchannel.o -lpthread -lrt

client: client.cpp reqchannel.o BoundedBuffer.o Histogram.o
	g++ -g -w -Wall -O0 -std=c++11 -o client client.cpp reqchannel.o BoundedBuffer.o Histogram.o -lpthread -lrt

clean:
	rm -rf *.o fifo* dataserver client
