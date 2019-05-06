# Dirs
OUT=bin/todo
INC=include
SRC_DIR=src
OBJ_DIR=bin/obj
ASM_DIR=bin/asm

# Compiler
CC=gcc
CFLAGS=-Wall -Werror -g -iquote $(INC) -std=c99

# Files
NAMES=todo command fileio main
SRC=$(patsubst %,$(SRC_DIR)/%.c,$(NAMES))
OBJ=$(patsubst %,$(OBJ_DIR)/%.o,$(NAMES))
ASM=$(patsubst %,$(ASM_DIR)/%.s,$(NAMES))

bin/todo: $(SRC)
	$(CC) -o $(OUT) $(CFLAGS) $(SRC)

obj: $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)
	@mv *.o $(OBJ_DIR)

asm: $(SRC)
	$(CC) $(CFLAGS) -S $(SRC)
	@mv *.s $(ASM_DIR)

pre-proc: $(SRC)
	@if [ ! -d "bin/proc" ]; then \
		mkdir bin/proc ;\
	fi
	$(CC) $(CFLAGS) -E $(SRC) > bin/proc/pre-proc.i

test:
	# @echo $(patsubst %,a_%.suffix,"name")

clean:
	rm $(OUT)
	rm `find . -name '*.o'`
	rm `find . -name '*.s'`
	rm `find . -name '*.i'`

all: asm obj pre-proc bin/todo

.PHONY: clean obj asm all pre-proc
