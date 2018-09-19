# makefile

all: shell

Main.o : Main.cpp
	g++ -c -g Main.cpp

shell: Main.o
	g++ -o shell Main.o

clean:
	rm *.o
	rm shell
