# Dirs
OUT=bin/todo
INC=include
SRC_DIR=src
OBJ_DIR=bin/obj
ASM_DIR=bin/asm

# Compiler
CC=gcc
CFLAGS=-Wall -Werror -g -iquote $(INC) -std=c99
LINT=uncrustify

# Files
UTIL=fileio io
NAMES=main todo command $(UTIL:%=util/%)
SRC=$(NAMES:%=$(SRC_DIR)/%.c)
OBJ=$(NAMES:%=$(OBJ_DIR)/%.o)
ASM=$(patsubst %,$(ASM_DIR)/%.s,$(NAMES))


$(OUT): $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(OBJ)

%.o: $(SRC)
	$(CC) $(CFLAGS) -c $^
	@for file in $(NAMES:%=%.o); do mv `basename $$file` $(OBJ_DIR)/$$file; done

%.c: setup

asm: $(SRC)
	$(CC) $(CFLAGS) -S $(SRC)
	@for file in $(NAMES:%=%.s); do mv `basename $$file` $(ASM_DIR)/$$file; done

pre-proc: $(SRC)
	@if [ ! -d "bin/proc" ]; then \
		mkdir bin/proc ;\
	fi
	$(CC) $(CFLAGS) -E $(SRC) > bin/proc/pre-proc.i

clean:
	@if [ -d $(OBJ_DIR) ]; then rm -rf $(OBJ_DIR); fi
	@if [ -d $(ASM_DIR) ]; then rm -rf $(ASM_DIR); fi
	@if [ -d bin/proc ]; then rm -rf bin/proc; fi
	@if [ -x $(OUT) ]; then rm $(OUT); fi
	rm `find . -name *.uncrustify`

setup:
	@if [ ! -d "$(OBJ_DIR)/util" ]; then mkdir $(OBJ_DIR)/util -p; fi
	@if [ ! -d "$(ASM_DIR)/util" ]; then mkdir $(ASM_DIR)/util -p; fi

fmt:
	@for file in $(SRC); do \
		$(LINT) -c lint.cfg -f $$file > $$file; \
	done

test:
	$(LINT)

all: setup asm pre-proc bin/todo

.PHONY: setup clean obj asm all pre-proc test fmt
