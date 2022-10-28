.PHONY: clean int double int_canary double_canary canary hash canary_and_hash int_hash double_hash int_canary_and_hash double_canary_and_hash

C_FLAGS = -Wall -Wextra -I includes/

all: main.o functions.o
	g++ $(C_FLAGS) bin/main.o bin/functions.o -o bin/exe
	bin/./exe

int:
	g++ $(C_FLAGS) -D INT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

double:
	g++ $(C_FLAGS) -D DOUBLE includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

int_canary:
	g++ $(C_FLAGS) -D INT -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

double_canary:
	g++ $(C_FLAGS) -D DOUBLE -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

int_hash:
	g++ $(C_FLAGS) -D INT -D HASH_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe	

double_hash:
	g++ $(C_FLAGS) -D DOUBLE -D HASH_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe	

int_canary_and_hash:
	g++ $(C_FLAGS) -D INT -D HASH_PROT -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

double_canary_and_hash:
	g++ $(C_FLAGS) -D DOUBLE -D HASH_PROT -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

canary: 
	g++ $(C_FLAGS) -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

hash:
	g++ $(C_FLAGS) -D HASH_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

canary_and_hash:
	g++ $(C_FLAGS) -D CANARY_PROT -D HASH_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

main.o: src/main.cpp includes/functions.h
	g++ $(C_FLAGS) -c src/main.cpp -o bin/main.o

functions.o: src/functions.cpp includes/functions.h
	g++ $(C_FLAGS) -c src/functions.cpp -o bin/functions.o

clean:
	rm bin/exe bin/functions.o bin/main.o