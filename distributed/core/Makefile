INCBASE = -I../..
FLAGS = -O2 -std=c++11

all: searcher proxy

proxy: proxy.cpp
	g++ $(FLAGS) $(INCBASE) -o proxy proxy.cpp -lzmq

searcher: main.cpp searcher.o
	g++ $(FLAGS) $(INCBASE) -o searcher main.cpp searcher.o ../proto/protocol.pb.o -lzmq -lprotobuf

searcher.o: searcher.h searcher.cpp
	g++ $(FLAGS) $(INCBASE) -c searcher.cpp

clear:
	rm *.o

