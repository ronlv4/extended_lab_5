OBJ_DIR := obj
BIN_DIR := bin
SRC_DIR := src
LIST_DIR := list

all: clean task0 looper task2

task0: #  $(OBJ_DIR)/lineParser.o
	gcc -g -m32 -Wall -o $(BIN_DIR)/myShell $(SRC_DIR)/myShell.c $(SRC_DIR)/lineParser.c

looper:
	gcc -g -m32 -Wall -o $(BIN_DIR)/looper $(SRC_DIR)/looper.c

task2: $(OBJ_DIR)/lineParser.o
	gcc -g -m32 -Wall -o $(BIN_DIR)/task2 $(SRC_DIR)/task2.c $(SRC_DIR)/lineParser.c

task2c: $(OBJ_DIR)/start.o $(OBJ_DIR)/util.o $(OBJ_DIR)/task2c.o
	ld -m elf_i386 $(OBJ_DIR)/start.o $(OBJ_DIR)/task2c.o $(OBJ_DIR)/util.o -o $(BIN_DIR)/task2c

$(OBJ_DIR)/lineParser.o: $(SRC_DIR)/lineParser.c
	gcc -m32 -Wall -c $(SRC_DIR)/lineParser.c -o $(OBJ_DIR)/lineParser.o
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/* $(LIST_DIR)/*
