INCBASE = -I..

indexer: main.cpp lister.o
	g++ $(INCBASE) -O2 -o indexer main.cpp lister.cpp

lister.o: lister.h lister.cpp
	g++ $(INCBASE) -O2 -c lister.cpp

clear:
	rm *.o

