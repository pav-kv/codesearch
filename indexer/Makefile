INCBASE = -I..
FLAGS = -g -O2 -pthread

indexer: main.cpp lister.o fileindex.o queue.o
	g++ $(FLAGS) $(INCBASE) -O2 -o indexer main.cpp lister.o fileindex.o queue.o

lister.o: lister.h lister.cpp
	g++ $(FLAGS) $(INCBASE) -O2 -c lister.cpp

fileindex.o: fileindex.h fileindex.cpp
	g++ $(FLAGS) $(INCBASE) -O2 -c fileindex.cpp

queue.o: queue.h queue.cpp
	g++ $(FLAGS) $(INCBASE) -O2 -c queue.cpp

clear:
	rm *.o
	rm indexer

