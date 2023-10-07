.PHONY: clean int double int_canary double_canary canary hash canary_and_hash int_hash double_hash int_canary_and_hash double_canary_and_hash

C_FLAGS = -Wall -Wextra -I includes/
CC = g++

all: bin/exe
	bin/./exe

bin/exe: bin/main.o bin/functions.o
	$(CC) $(C_FLAGS) bin/main.o bin/functions.o -o bin/exe

int:
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D INT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

double:
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D DOUBLE includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

int_canary:
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D INT -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

double_canary:
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D DOUBLE -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

int_hash:
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D INT -D HASH_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe	

double_hash:
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D DOUBLE -D HASH_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe	

int_canary_and_hash:
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D INT -D HASH_PROT -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

double_canary_and_hash:
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D DOUBLE -D HASH_PROT -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

canary: 
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D CANARY_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

hash:
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D HASH_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

canary_and_hash:
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -D CANARY_PROT -D HASH_PROT includes/functions.h src/functions.cpp src/main.cpp -o bin/exe
	bin/./exe

bin/main.o: src/main.cpp includes/functions.h
	if [ ! -d bin ]; then mkdir bin; fi
	$(CC) $(C_FLAGS) -c src/main.cpp -o bin/main.o

bin/functions.o: src/functions.cpp includes/functions.h
	$(CC) -I includes/ -c src/functions.cpp -o bin/functions.o

clean:
	rm -r bin/