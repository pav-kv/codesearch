INCBASE = -I../..
FLAGS = -O2 -std=c++11

all: cscore csproxy

csproxy: proxy.cpp
	g++ $(FLAGS) $(INCBASE) -o csproxy proxy.cpp -lzmq

cscore: indexer.h main.cpp searcher.cpp searcher.h server.h
	g++ $(FLAGS) $(INCBASE) -o cscore main.cpp searcher.cpp ../../index/writer.o ../../indexer/lister.o ../proto/protocol.pb.o -lzmq -lprotobuf -lboost_filesystem

clean:
	rm *.o

