#include <stdio.h>  // printf
#include <stdlib.h>  // malloc, calloc, realloc
#include <inttypes.h> // strtoumax, strtoimax

#include "command.h" // CMD, addCommand, setToplevel, parse
#include "fileio.h"  // file_len, file_lines

#define true 1
#define false 0


CMD todo = {
	.use = "todo",
	.descr = "write down your todo list.",
};

void print_todo() {
	FILE *fpt = fopen("./TODO", "r");
	if (fpt == NULL) {
		printf("creating 'TODO' file\n");
		FILE *f = fopen("./TODO", "w");
		fclose(f);
	}

	int flen = file_len("./TODO");
	if (flen == 1) {
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

void add_note(int argc, char** args) {
	FILE *fpt = fopen("./TODO", "a+");
	int flen = file_lines("./TODO");
	fprintf(fpt, "%d. ", ++flen);
	for (int i = 0; i < argc; i++) {
		fprintf(fpt, "%s ", args[i]);
	}
	fprintf(fpt, "\n");
	fclose(fpt);
	print_todo();
}

void run_rm(CMD *cmd, int argc, char** args) {
	if (argc > 1) {
		printf("too many arguments\n");
		exit(1);
	}

	FILE *fpt = fopen("./TODO", "w+");
	char *end;
	// printf("%d\n", strtoumax(args[0], &end, 10));
	int line = strtoumax(args[0], &end, 10);
	fseek_line(fpt, line);

	// fputs("line replacement", fpt);
	// print_nextline(fpt);

	// for (int i = 0; i < argc; i++) {
	// 	// printf("removing '%s'...\n", args[i]);
	//
	// 	free(args[i]);
	// }
	fclose(fpt);
}

CMD rm = {
	.use = "rm <line>",
	.descr = "remove an item from the list",
	.run = run_rm,
	.hasargs = true
};

void get_run(CMD *cmd, int argc, char** args) {
	if (argc > 1) {
		printf("too many arguments\n");
		exit(1);
	}
	FILE *fpt = fopen("./TODO", "a+");

	char *end;
	fseek_line(fpt, strtoumax(args[0], &end, 10));
	print_nextline(fpt);
	fclose(fpt);
}

CMD get = {
	.use = "get <line>",
	.descr = "show the <n>th item on the list",
	.run = get_run
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

void run_test(CMD *cmd, int argc, char** argv) {
	printf("[");
	for (int i = 0; i < argc; i++) {
		if (i == argc - 1)
			printf("%s", argv[i]);
		else
			printf("%s ", argv[i]);
	}
	printf("]\n");
}

CMD test = {
	.use = "test",
	.descr = "developer testing option",
	.run = run_test,
	.hidden = true
};

static void init() {
	setRoot(&todo);

	addCommand(&rm);
	addCommand(&del);
	addCommand(&get);

	addCommand(&test);
}

int main(int argc, char *argv[]) {
	init();

	if (!parse(argc, argv)) {
		if (argc == 1) {
			print_todo();
		} else {
			// char** args = getSlice(argc, argv, 1);
			// free(args);
			add_note(argc - 1, ++argv);
		}
	}
	return 0;
}
