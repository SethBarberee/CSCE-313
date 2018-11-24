# makefile

all: dataserver client

reqchannel.o: reqchannel.h reqchannel.cpp
	g++ -g -w -Wall -O0 -std=c++11 -c reqchannel.cpp

fifochannel.o: fifochannel.h fifochannel.cpp
	g++ -g -w -Wall -O0 -std=c++11 -c fifochannel.cpp

mqchannel.o: mqchannel.h mqchannel.cpp
	g++ -g -w -Wall -O0 -std=c++11 -c mqchannel.cpp

shmchannel.o: shmchannel.h shmchannel.cpp
	g++ -g -w -Wall -O0 -std=c++11 -c shmchannel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -g -w -Wall -O0 -std=c++11 -c BoundedBuffer.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -Wall -O0 -std=c++11 -c Histogram.cpp

dataserver: dataserver.cpp reqchannel.o fifochannel.o mqchannel.o shmchannel.o
	g++ -g -w -Wall -O0 -std=c++11 -o dataserver dataserver.cpp reqchannel.o fifochannel.o mqchannel.o shmchannel.o -lpthread -lrt

client: client.cpp reqchannel.o fifochannel.o mqchannel.o shmchannel.o BoundedBuffer.o Histogram.o
	g++ -g -w -Wall -O0 -std=c++11 -o client client.cpp reqchannel.o fifochannel.o mqchannel.o shmchannel.o BoundedBuffer.o Histogram.o -lpthread -lrt

clean:
	rm -rf *.o fifo_* mq_* shm_* KemSem* dataserver client
