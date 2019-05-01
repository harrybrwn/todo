CC=gcc
CFLAGS=-iquote include

OUT=bin/todo
OBJ_DIR=obj
SRC_DIR=src

FILES=todo.c command.c

OBJS=$(patsubst %,$(OBJ_DIR)/%,$(patsubst %.c,%.o, $(FILES)))
SRC=$(patsubst %,$(SRC_DIR)/%,$(FILES))

todo: %.o
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT)

%.o:
	$(CC) $(CFLAGS) -c $(SRC)
	@mv *.o $(OBJ_DIR)

.PHONY: out
