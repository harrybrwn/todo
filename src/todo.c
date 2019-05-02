#include <stdio.h>  // printf
#include <stdlib.h>  // malloc, calloc, realloc

#include "command.h" // CMD, addCommand, setToplevel, parse
#include "fileio.h"  // file_len, file_lines

#define true 1
#define false 0


CMD todo = {
	.use = "todo",
	.descr = "write down your todo list.",
};

void run_add(CMD *cmd, int argc, char** args) {
	FILE *fpt = fopen("./TODO", "a+");
	int flen = file_lines("./TODO");

	for (int i = 0; i < argc; i++) {
		fprintf(fpt, "%d. %s\n", ++flen, args[i]);
	}
	fclose(fpt);
}

CMD add = {
	.use = "add",
	.descr = "add an item to the list",
	.run = run_add,
	.hasargs = true
};

void run_rm(CMD *cmd, int argc, char** args) {
	for (int i = 0; i < argc; i++) {
		printf("removing '%s'...\n", args[i]);
		free(args[i]);
	}
}

CMD rm = {
	.use = "rm",
	.descr = "remove an item from the list",
	.run = run_rm,
	.hasargs = true
};

void run_delete(CMD *cmd, int argc, char** args) {
	for (int i = 0; i < argc; i++) {
		free(args[i]);
	}
	int status = remove("./TODO");
	if (status == 0)
		printf("deleted TODO.\n");
	else
		printf("cannot delete 'TODO' file\n");
}

CMD del = {
	.use = "del",
	.descr = "delete the current TODO file",
	.run = run_delete,
	.hasargs = false
};

void print_file() {
	FILE *fpt = fopen("./TODO", "r");
	if (fpt == NULL) {
		printf("creating 'TODO' file\n");
		FILE *f = fopen("./TODO", "w");
		fclose(f);
	}

	int flen = file_len("./TODO");
	if (flen == 1) {
		printf("empty file has %d lines\n", file_lines("./TODO"));
		printf("your TODO is empty\n");
		return;
	}

	char c;
	while (c != EOF) {
		c = fgetc(fpt);
		printf("%c", c);
	}
	fclose(fpt);
}

void init() {
	setRoot(&todo);
	addCommand(&add);
	addCommand(&rm);
	addCommand(&del);
}

int main(int argc, char *argv[]) {
	init();

	if (!parse(argc, argv)) {
		if (argc == 1) {
			print_file();
		} else {
			char** args = getSlice(argc, argv, 1);
			run_add(&add, 1, args);
			free(args);
		}
	}
	return 0;
}
