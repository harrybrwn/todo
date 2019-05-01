# Dirs
OUT=bin/todo
SRC_DIR=src
INC=include
OBJ_DIR=bin/obj
ASM_DIR=bin/asm

# Compiler
CC=gcc
CFLAGS=-iquote $(INC)

# Files
NAMES=todo command
SRC=$(patsubst %,$(SRC_DIR)/%.c,$(NAMES))
OBJS=$(patsubst %,$(OBJ_DIR)/%.o,$(NAMES))
ASM=$(patsubst %,$(ASM_DIR)/%.s,$(NAMES))


bin/todo: $(OBJ_DIR)/todo.o
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT)

%.o: %.s
	$(CC) $(CFLAGS) -c $(ASM)
	@mv *.o $(OBJ_DIR)

%.s: $(SRC) $(INC)/command.h
	$(CC) $(CFLAGS) -S $(SRC)
	@mv *.s $(ASM_DIR)

clean: base
	rm $(OUT)
	rm `find . -name '*.o'`
	rm `find . -name '*.s'`

raw: base
	$(CC) $(CFLAGS) -E $(SRC) > raw_output.i

imports-paths: base
	@echo $(SRC)
	$(CC) $(CFLAGS) -E -M $(SRC)

base: $(SRC) $(INC)/command.h

.PHONY: clean raw base imports-paths
