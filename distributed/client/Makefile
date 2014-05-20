INCBASE = -I../..
FLAGS = -O2 -std=c++11

csclient: main.cpp
	g++ $(FLAGS) $(INCBASE) -o csclient main.cpp ../proto/protocol.pb.o -lzmq -lprotobuf

clear:
	rm *.o

