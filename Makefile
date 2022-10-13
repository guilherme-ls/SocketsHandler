CC= g++
CFLAGS= -Wall -g -I.
TARGET= main
CFILES= sockets.cpp
HFILES= sockets.hpp 

all :
	$(CC) $(CFLAGS) main_daemon.cpp $(CFILES) -o main
	$(CC) $(CFLAGS) client1.cpp $(CFILES) -o client1
	$(CC) $(CFLAGS) client2.cpp $(CFILES) -o client2
	$(CC) $(CFLAGS) client3.cpp $(CFILES) -o client3

run :
	./$(TARGET)