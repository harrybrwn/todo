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
OBJ=$(patsubst %,$(OBJ_DIR)/%.o,$(NAMES))
ASM=$(patsubst %,$(ASM_DIR)/%.s,$(NAMES))

bin/todo:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)


# bin/todo: $(OBJ_DIR)/todo.o $(OBJ_DIR)/command.o
# 	$(CC) $(CFLAGS) $(OBJ) -o $(OUT)

# %.o: %.s
# 	$(CC) $(CFLAGS) -c $(ASM)
# 	@mv *.o $(OBJ_DIR)
#
# %.s: $(SRC)
# 	$(CC) $(CFLAGS) -S $(SRC)
# 	@mv *.s $(ASM_DIR)

test: test.c
	$(CC) -save-temps test.c -o test

clean-test:
	rm test
	rm test.i
	rm test.s
	rm test.o

clean:
	rm $(OUT)
	rm `find . -name '*.o'`
	rm `find . -name '*.s'`

.PHONY: clean
