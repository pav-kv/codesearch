INCBASE = -I..
FLAGS = -O2 -std=c++11

searcher: main.cpp query.h searcher.o
	g++ $(FLAGS) $(INCBASE) -o searcher main.cpp searcher.o

searcher.o: searcher.h searcher.cpp
	g++ $(FLAGS) $(INCBASE) -c searcher.cpp

clear:
	rm *.o

