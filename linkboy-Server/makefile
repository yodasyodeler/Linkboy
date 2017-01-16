OBJS= common.o linkboyServer.o main.o 
CC=g++

VER= -std=gnu++11 -pthread

CFLAGS=-Wall -c $(VER)

LIBS=-lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
INCDIR=-I../include
EXECUTABLE=linkboyServer

all: linkboyServer

linkboyServer: $(OBJS)
	$(CC) $(VER) $(OBJS) $(INCDIR) -o $(EXECUTABLE) $(LIBS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp $(INCDIR)

linkboyServer.o: linkboyServer.cpp linkboyServer.h
	$(CC) $(CFLAGS) linkboyServer.cpp  $(INCDIR)

common.o: ../source/common.cpp ../include/common.h
	$(CC) $(CFLAGS) ../source/common.cpp $(INCDIR) 

clean:
	rm $(EXECUTABLE) *.o