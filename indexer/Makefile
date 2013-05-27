INCBASE = -I..
FLAGS = -g -O2 -std=c++11

indexer: main.cpp lister.o indexer.o
	g++ $(FLAGS) $(INCBASE) -o indexer main.cpp lister.o indexer.o

lister.o: lister.h lister.cpp
	g++ $(FLAGS) $(INCBASE) -c lister.cpp

indexer.o: indexer.h indexer.cpp
	g++ $(FLAGS) $(INCBASE) -c indexer.cpp

clear:
	rm *.o

