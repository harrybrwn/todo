#include <stdio.h>  // printf
#include <stdlib.h>
#include <inttypes.h> // strtoumax, strtoimax

#include "command.h" // CMD, addCommand, setToplevel, parse_opts
#include "fileio.h"  // file_len, file_lines
#include "todo.h"

static CMD todo = {
	.use = "todo",
	.descr = "write down your todo list.",
};

static void print_todo() {
	TODO *todof = open_todo("./TODO", "r");

	if (todof->stream == NULL) {
		printf("creating 'TODO' file\n");
		FILE *f = fopen("./TODO", "w");
		fclose(f);
		return;
	}

	if (todof->length == 1) {
		printf("your TODO is empty\n");
		return;
	}

	char c;
	while ((c = fgetc(todof->stream)) != EOF)
		printf("%c", c);

	close_todo(&todof);
}

static void add_note(int argc, char** args) {
	FILE *fpt = fopen("./TODO", "a+");
	int flen = file_lines("./TODO");
	fprintf(fpt, "%d. ", ++flen);

	for (int i = 0; i < argc; i++)
		fprintf(fpt, "%s ", args[i]);

	fprintf(fpt, "\n");
	fclose(fpt);
	print_todo();
}

static void error(const char* msg) {
	printf("%s\n", msg);
	exit(1);
}

static int parse_int(char *str) {
	char *end;
	return strtoumax(str, &end, 10);
}

static void run_rm(CMD *cmd, int argc, char** argv) {
	// if (argc > 1) {
	// 	error("too many arguments");
	// }
	// int line_no = parse_int(argv[0])
	//
	// FILE *fpt = fopen("./TODO", "r+");
	// FileInfo* info_ptr = get_info(fpt);
	// FileInfo info = *info_ptr;
	// close_info(info_ptr);
	//
	// char *data = malloc(info.length);
	//
	// char c;
	// int line = 1;
	// while ((c = fgetc(fpt)) != EOF) {
	// 	if (c == '\n') {
	//
	// 		line++;
	// 	}
	// 	// if (line == line_no)
	// }
	//
	// free(data);

	// fclose(fpt);
	error("rm does not work yet");
}

static CMD rm = {
	.use = "rm <line>",
	.descr = "remove an item from the list",
	.run = run_rm,
	.hasargs = true
};

static void run_get(CMD *cmd, int argc, char** args) {
	if (argc > 1) {
		error("Error: too many arguments");
	} else if (argc < 1) {
		error("Error: give line number");
	}

	FILE *fpt = fopen("./TODO", "r");

	fseek_line(fpt, parse_int(args[0]) - 1);
	print_nextline(fpt);
	fclose(fpt);
}

static CMD get = {
	.use = "get <line>",
	.descr = "show the <n>th item on the list",
	.run = run_get
};

static void run_delete(CMD *cmd, int argc, char** args) {
	for (int i = 0; i < argc; i++) {
		free(args[i]);
	}

	if (remove("./TODO") == 0)
		printf("deleted TODO.\n");
	else
		printf("cannot delete 'TODO' file\n");
}

static CMD del = {
	.use = "del",
	.descr = "delete the current TODO file",
	.run = run_delete,
	.hasargs = false
};

static void run_test(CMD *cmd, int argc, char** argv) {
	FILE *fpt = fopen("./TODO", "r");

	char *end;
	fseek_line(fpt, strtoumax(argv[0], &end, 10) - 1);
	char c;

	while ((c = fgetc(fpt)) != EOF) {
		if (c == '\n')
			break;
		printf("%c", c);
	}

	printf("\n");
	fclose(fpt);
}

static CMD test = {
	.use = "test",
	.descr = "developer testing option",
	.run = run_test,
	.hidden = true
};

static void init() {
	setRoot(&todo);

	// top level
	addCommand(&rm);
	addCommand(&del);
	addCommand(&get);

	// hidden
	addCommand(&test);
}

int main(int argc, char *argv[]) {
	init();

	if (!parse_opts(argc, argv)) {
		if (argc == 1) {
			print_todo();
		} else {
			add_note(--argc, ++argv);
		}
	}
	return 0;
}
