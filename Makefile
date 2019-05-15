# Dirs
OUT=bin/todo
INC=include
SRC_DIR=src
OBJ_DIR=bin/obj
ASM_DIR=bin/asm

# Compiler and Code Analysis
CC=gcc
CFLAGS=-Wall -Werror -g -iquote $(INC) -std=c99
FMT=uncrustify
FMT_CONFIG=lint.cfg


# Files
UTIL=fileio io map stack
COMMAND=cmd flag
PKG=todo $(UTIL:%=util/%) $(COMMAND:%=command/%)
NAMES=main $(PKG)
SRC=$(NAMES:%=$(SRC_DIR)/%.c)
HEADERS=$(PKG:%=$(INC)/%.h)

# Testing
TEST=tests/test


$(OUT): $(SRC) $(HEADERS)
	$(CC) -o $(OUT) $(CFLAGS) $(SRC)

obj: $(SRC)
	$(CC) $(CFLAGS) -c $^
	@for file in $(NAMES:%=$(OBJ_DIR)/%.o); do mv `basename $$file` $$file; done

asm: $(SRC)
	$(CC) $(CFLAGS) -S $(SRC)
	@for file in $(NAMES:%=%.s); do mv `basename $$file` $(ASM_DIR)/$$file; done

bin/proc/pre-proc.i: $(SRC)
	@if [ ! -d "bin/proc" ]; then \
		mkdir bin/proc ;\
	fi
	$(CC) $(CFLAGS) -E $(SRC) > $@

clean:
	@for file in `find . -name *.o`; do rm $$file; done
	@for file in `find . -name *.s`; do rm $$file; done
	@for file in `find . -name *.uncrustify`; do rm $$file; done
	@if [ -x $(OUT) ]; then rm $(OUT); fi

setup:
	@if [ ! -d "$(OBJ_DIR)/util" ]; then mkdir $(OBJ_DIR)/util -p; fi
	@if [ ! -d "$(ASM_DIR)/util" ]; then mkdir $(ASM_DIR)/util -p; fi

fmt: lint
	@for file in $(SRC) `find $(INC) -name *.h`; do \
		$(FMT) -c $(FMT_CONFIG) -f $$file > $$file.lint; \
		diff $$file $$file.lint; \
		cat $$file.lint > $$file; \
		rm $$file.lint; \
	done

lint:
	@go run ./scripts/lint.go "$(CFLAGS) $(SRC)" -show-cmd

test:
	@$(CC) $(CFLAGS) -o $(TEST) $(PKG:%=$(SRC_DIR)/%.c) tests/test.c
	@$(TEST) this is a test with args

MEM=valgrind --leak-check=full

check: $(OUT)
	$(MEM) $(OUT)
	@echo
	$(MEM) $(OUT) this is a test note
	@echo
	$(MEM) $(OUT) --file test.txt


all: setup asm obj bin/proc/pre-proc.i bin/todo

.PHONY: setup clean obj asm all pre-proc test fmt lint check
