INCLUDE = ./include
SRC = ./src
CC = c++
BIN_DIR = ./bin
CFLAGS = -g -Wall

sicasm:
	$(CC) $(SRC)/sic_asm.cpp -I$(INCLUDE)/ -o$(BIN_DIR)/sic_asm

clean:
	rm -f $(BIN_DIR)/*