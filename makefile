.PHONY: clean int 

all: main.o functions.o
	g++ main.o functions.o -o exe
	./exe

int:
	g++ -Wall -Wextra -D INT functions.h functions.cpp main.cpp -o exe
	./exe

main.o: main.cpp
	g++ -Wall -Wextra -c main.cpp -o main.o

functions.o: functions.cpp
	g++ -Wall -Wextra -c functions.cpp -o functions.o

clean:
	rm functions.o main.o exe