INCBASE = -I../..
FLAGS = -O2 -std=c++11

client: main.cpp
	g++ $(FLAGS) $(INCBASE) -o client main.cpp ../proto/protocol.pb.o -lzmq -lprotobuf

clear:
	rm *.o

