.PHONY: clean int double int_canary double_canary canary

all: main.o functions.o
	g++ bin/main.o bin/functions.o -o bin/exe
	bin/./exe

int:
	g++ -Wall -Wextra -D INT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

double:
	g++ -Wall -Wextra -D DOUBLE includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

int_canary:
	g++ -Wall -Wextra -D INT -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

double_canary:
	g++ -Wall -Wextra -D DOUBLE -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

canary: 
	g++ -Wall -Wextra -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

main.o: src/main.cpp
	g++ -Wall -Wextra -c src/main.cpp -o bin/main.o

functions.o: src/functions.cpp
	g++ -Wall -Wextra -c src/functions.cpp -o bin/functions.o

clean:
	rm bin/exe bin/functions.o bin/main.o output_files/data.txt