# Dirs
OUT=bin/todo
INC=include
SRC_DIR=src
OBJ_DIR=bin/obj
ASM_DIR=bin/asm

# Compiler
CC=gcc
CFLAGS=-Wall -Werror -g -iquote $(INC) -std=c99
FMT=uncrustify

# Files
UTIL=fileio io
NAMES=main todo command $(UTIL:%=util/%)
SRC=$(NAMES:%=$(SRC_DIR)/%.c)
OBJ=$(NAMES:%=$(OBJ_DIR)/%.o)


$(OUT): $(SRC)
	$(CC) -o $(OUT) $(CFLAGS) $(SRC)

obj: $(SRC)
	$(CC) $(CFLAGS) -c $^
	@for file in $(NAMES:%=%.o); do mv `basename $$file` $(OBJ_DIR)/$$file; done

asm: $(SRC)
	$(CC) $(CFLAGS) -S $(SRC)
	@for file in $(NAMES:%=%.s); do mv `basename $$file` $(ASM_DIR)/$$file; done

bin/proc/pre-proc.i: $(SRC)
	@if [ ! -d "bin/proc" ]; then \
		mkdir bin/proc ;\
	fi
	$(CC) $(CFLAGS) -E $(SRC) > $@ # bin/proc/pre-proc.i

clean:
	@for file in `find . -name *.o`; do rm $$file; done
	@for file in `find . -name *.s`; do rm $$file; done
	@for file in `find . -name *.uncrustify`; do rm $$file; done
	@if [ -x $(OUT) ]; then rm $(OUT); fi

setup:
	@if [ ! -d "$(OBJ_DIR)/util" ]; then mkdir $(OBJ_DIR)/util -p; fi
	@if [ ! -d "$(ASM_DIR)/util" ]; then mkdir $(ASM_DIR)/util -p; fi

fmt:
	@for file in $(SRC) `find $(INC) -name *.h`; do \
		$(FMT) -c lint.cfg -f $$file > $$file.lint; \
		diff $$file $$file.lint; \
		cat $$file.lint > $$file; \
		rm $$file.lint; \
	done

all: setup asm obj bin/proc/pre-proc.i bin/todo

.PHONY: setup clean obj asm all pre-proc test fmt
