INCLUDE = ./include
SRC = ./src
CC = c++
BIN_DIR = ./bin
CFLAGS = -g -Wall -std=c++11

sicasm:
	$(CC) $(CFLAGS) $(SRC)/sic_asm.cpp -I$(INCLUDE)/ -o$(BIN_DIR)/sic_asm

clean:
	rm -f $(BIN_DIR)/*
