#include <stdio.h>  // printf
#include <stdlib.h>
#include <inttypes.h> // strtoumax, strtoimax
#include <string.h>

#include "command.h" // CMD, addCommand, setToplevel, parse_opts
#include "fileio.h"  // file_len, file_lines
#include "todo.h"

static CMD todo = {
	.use = "todo",
	.descr = "write down your todo list.",
};

static void show_todo() {
	TODO *todof = open_todo("./TODO", "r");

	if (todof->stream == NULL) {
		printf("creating 'TODO' file\n");
		FILE *f = fopen("./TODO", "w");
		fclose(f);
		return;
	}

	print_todo(todof);
	close_todo(&todof);
}

static void add_note(int argc, char** args) {
	TODO* todof = open_todo("./TODO", "r+");

	int buf_size = 32;
	char* buf = malloc(buf_size);

	for (int i = 0; i < argc; i++) {
		if (strlen(buf) >= buf_size - 1) {
			buf_size += 16;
			buf = realloc(buf, buf_size);
		}
		strcat(buf, args[i]);

		if (i != argc - 1)
			strcat(buf, " ");
	}

	Note* n = new_note(todof->lines + 1, buf, NULL);

	write_note(n, todof->stream);
	close_todo(&todof);
	close_note(&n);
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
	if (argc > 1) {
		error("too many arguments");
	} else if (argc < 1) {
		error("Error: must give note number");
	}

	TODO* todof = open_todo("./TODO", "r+");
	int line_index = parse_int(argv[0]) - 1;
	printf("line-index: %d, todo-lines: %d\n", line_index, todof->lines);
	if (line_index > todof->lines)
		error("Error: todo file is not that long");

	todof->notes[line_index] = NULL;
	fclose(todof->stream);

	todof->stream = fopen("./TODO", "w+");
	if (todof->length == 1)
		todof->length = 2;

	print_todo(todof);
	write_todo(todof);
	close_todo(&todof);
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

	TODO* todof = open_todo("./TODO", "r");

	int index = parse_int(args[0]);
	if (index <= 0)
		error("Error: please give a line number");
	else if (index > todof->lines)
		error("Error: todo list isn't that long");

	printf("line %d:\n", index);
	printf("\t%s\n", todof->notes[index - 1]->note);
	close_todo(&todof);
}

static CMD get = {
	.use = "get <line>",
	.descr = "show the <n>th item on the list",
	.run = run_get
};

static void run_delete(CMD *cmd, int argc, char** args) {
	for (int i = 0; i < argc; i++)
		free(args[i]);

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
	TODO *todof = open_todo("./TODO", "r");
	todof->notes = malloc(todof->lines * sizeof(Note*));

	for (int i = 0; i < todof->lines; i++) {
		todof->notes[i] = read_note(todof->stream);
		printf("note #%d (%d bytes long): %s\n", todof->notes[i]->line, todof->notes[i]->length, todof->notes[i]->note);
	}

	close_todo(&todof);
}

static CMD test = {
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

	// hidden
	addCommand(&test);
}

int main(int argc, char *argv[]) {
	init();

	if (!parse_opts(argc, argv)) {
		if (argc == 1) {
			show_todo();
		} else {
			add_note(--argc, ++argv);
		}
	}
	return 0;
}
