#include <stdio.h>  // printf
#include <stdlib.h>  // malloc, calloc, realloc

#include "command.h"  // CMD, addCommand, setToplevel, parse

#define true 1
#define false 0


CMD todo = {
	.use = "todo",
	.descr = "",
};

void run_add(CMD *cmd, int argc, char** args) {
	FILE *fpt = fopen("./TODO", "a+");
	for (int i = 0; i < argc; i++) {
		printf("adding '%s' to the list...\n", args[i]);
		fprintf(fpt, "%s\n", args[i]);
		free(args[i]);
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

void run_show(CMD *cmd, int argc, char** args) {
	for (int i = 0; i < argc; i++) {
		free(args[i]);
	}
	printf("you have nothing in your list\n");
}

CMD show = {
	.use = "show",
	.descr = "show an item on your list",
	.run = run_show,
	.hasargs = false
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

void cat() {
	FILE *fpt = fopen("./TODO", "r");
	if (fpt == NULL) {
		printf("could not read 'TODO' file\n");
		fclose(fpt);
		FILE *f = fopen("./TODO", "w");
		fclose(fpt);
		exit(1);
	}
	char c = fgetc(fpt);
	while (c != EOF) {
		printf("%c", c);
		c = fgetc(fpt);
	}
	fclose(fpt);
}

void init() {
	setRoot(&todo);
	addCommand(&add);
	addCommand(&rm);
	addCommand(&show);
	addCommand(&del);
}

int main(int argc, char *argv[]) {
	init();

	if (!parse(argc, argv)) {
		if (argc == 1)
			cat();
		else
			run_add(&add, 1, getSlice(argc, argv, 1));
	}
	return 0;
}
