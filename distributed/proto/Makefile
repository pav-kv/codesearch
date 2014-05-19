INCBASE = -I../..
FLAGS = -O2 -std=c++11

all: metadata.pb.o protocol.pb.o

metadata.pb.o: metadata.pb.h metadata.pb.cc
	g++ $(FLAGS) $(INCBASE) -c metadata.pb.cc

protocol.pb.o: protocol.pb.h protocol.pb.cc
	g++ $(FLAGS) $(INCBASE) -c protocol.pb.cc

metadata.pb.h metadata.pb.cc: metadata.proto
	protoc --cpp_out=. metadata.proto

protocol.pb.h protocol.pb.cc: protocol.proto
	protoc --cpp_out=. protocol.proto

clean:
	rm *.pb.h *.pb.cc *.o

