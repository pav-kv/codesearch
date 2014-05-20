INCBASE = -I..
FLAGS = -O2 -std=c++11

indexer: main.cpp lister.o
	g++ $(FLAGS) $(INCBASE) -o indexer main.cpp lister.o ../index/writer.o

lister.o: lister.h lister.cpp
	g++ $(FLAGS) $(INCBASE) -c lister.cpp

clean:
	rm *.o

