#include <stdio.h>  // printf
#include <stdlib.h>  // malloc, calloc, realloc

#include "command.h"

#define true 1
#define false 0

void run_add(CMD *cmd, int argc, char** args) {
	for (int i = 0; i < argc; i++) {
		printf("adding %s to the list...\n", args[i]);
		free(args[i]);
	}
}

CMD todo = {
	.use = "todo",
	.descr = ""
};

CMD add = {
	.use = "add",
	.descr = "add an item to the list",
	.run = run_add,
	.hasargs = true
};

void run_rm(CMD *cmd, int argc, char** args) {
	for (int i = 0; i < argc; i++) {
		printf("removing %s...\n", args[i]);
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
	printf("you have nothing on your list\n");
}

CMD show = {
	.use = "show",
	.descr = "show an item on your list",
	.run = run_show,
	.hasargs = false
};

int main(int argc, char *argv[]) {
	setToplevel(&todo);
	addCommand(&add);
	addCommand(&rm);
	addCommand(&show);

	if (!parse(argc, argv)) {
		printf("\nError: no valid command\n");
	}
	return 0;
}
