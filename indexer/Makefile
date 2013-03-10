INCBASE = -I..
FLAGS = -g -O2 -pthread -std=c++0x

indexer: main.cpp lister.o fileindex.o queue.o indexer.o
	g++ $(FLAGS) $(INCBASE) -O2 -o indexer main.cpp lister.o fileindex.o queue.o

lister.o: lister.h lister.cpp
	g++ $(FLAGS) $(INCBASE) -O2 -c lister.cpp

fileindex.o: fileindex.h fileindex.cpp
	g++ $(FLAGS) $(INCBASE) -O2 -c fileindex.cpp

queue.o: queue.h queue.cpp
	g++ $(FLAGS) $(INCBASE) -O2 -c queue.cpp

indexer.o: indexer.h indexer.cpp
	g++ $(FLAGS) $(INCBASE) -O2 -c indexer.cpp

clear:
	rm *.o
	rm indexer

