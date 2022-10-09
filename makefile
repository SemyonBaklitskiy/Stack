.PHONY: clean int double int_canary double_canary

all: main.o functions.o
	g++ main.o functions.o -o exe
	./exe

int:
	g++ -Wall -Wextra -D INT functions.h functions.cpp main.cpp -o exe
	./exe

double:
	g++ -Wall -Wextra -D DOUBLE functions.h functions.cpp main.cpp -o exe
	./exe

int_canary:
	g++ -Wall -Wextra -D INT -D CANARY_PROT functions.h functions.cpp main.cpp -o exe
	./exe

double_canary:
	g++ -Wall -Wextra -D DOUBLE -D CANARY_PROT functions.h functions.cpp main.cpp -o exe
	./exe

main.o: main.cpp
	g++ -Wall -Wextra -c main.cpp -o main.o

functions.o: functions.cpp
	g++ -Wall -Wextra -c functions.cpp -o functions.o

clean:
	rm exe