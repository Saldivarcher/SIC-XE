CC = c++
CFLAGS = -std=c++11 -g

all: assembler

assembler: assembler.o main.o
	$(CC) $(CFLAGS) assembler.o main.o -o assembler_xe

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

assembler.o: assembler.cpp
	$(CC) $(CFLAGS) -c assembler.cpp

clean:
	rm *.list *.o *.interm assembler_xe

