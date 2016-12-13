CC = c++
CFLAGS = -std=c++11 

all: linker

linker: linker.o main.o
	$(CC) $(CFLAGS) linker.o main.o -o link

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

linker.o: linker.cpp
	$(CC) $(CFLAGS) -c linker.cpp

clean:
	rm *.o link
