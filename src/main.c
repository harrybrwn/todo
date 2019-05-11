#include <stdio.h>  // printf
#include <stdlib.h>
#include <inttypes.h> // strtoumax, strtoimax
#include <string.h>

#include "command.h"
#include "util/fileio.h"
#include "util/io.h"
#include "todo.h"

static const char* TODOFILE = "./TODO";

void show_todo(const char* filename) {
	TODO* todof = open_todo(filename, "r");

	if (todof->stream == NULL) {
		printf("creating 'TODO' file\n");
		FILE* f = fopen(filename, "w");

		fclose(f);
		return;
	}
	print_todo(todof);
	close_todo(&todof);
}

static void add_note(CMD* cmd, int argc, char** args) {
	const char* file = TODOFILE;

	for (int i = 0; i < cmd->n_flags; i++) {
		if (cmd->flags[i].triggered && strcmp(cmd->flags[i].name, "file") == 0) {
			file = (char*)cmd->flags[i].value;
		}
	}

	if (argc == 0) {
		show_todo(file);
		return;
	}

	TODO*   todof = open_todo(file, "r+");
	Buffer* buf = new_buffer(32);

	for (int i = 0; i < argc; i++) {
		bufputs(buf, args[i]);
		if (i != argc - 1) {
			bufputc(buf, ' ');
		}
	}
	bufputc(buf, '\0');

	Note* n = new_note(todof->lines + 1, buf->data, NULL);
	write_note(n, todof->stream);
	close_todo(&todof);
	close_note(&n);
}

static void error(const char* msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

static int parse_int(char* str) {
	char* end;

	return strtoumax(str, &end, 10);
}

static void run_rm(CMD* cmd, int argc, char** argv) {
	if (argc > 1) {
		error("too many arguments");
	} else if (argc < 1) {
		error("Error: must give note number");
	}

	TODO* todof = open_todo("./TODO", "r+");
	int   line_index = parse_int(argv[0]) - 1;

	if (line_index > todof->lines) {
		error("Error: todo file is not that long");
	}

	todof->notes[line_index] = NULL;
	if (todof->length == 1) {
		todof->length = 2;
	}

	print_todo(todof);
	write_todo(todof);
	close_todo(&todof);
}

static CMD rm = {
	.use   = "rm    <line>",
	.descr = "Remove an item from the list",
	.run   = run_rm,
};

void run_check(CMD* cmd, int argc, char** argv) {
	if (argc > 1) {
		error("too many arguments");
	} else if (argc < 1) {
		error("Error: must give note number");
	}

	TODO* todof = open_todo("./TODO", "r+");
	int   line_index = parse_int(argv[0]) - 1;

	if (line_index > todof->lines) {
		error("Error: todo file is not that long");
	}

	Buffer* buf = new_buffer(32);

	int len = strlen(todof->notes[line_index]->note);

	for (int i = 0; i < len; i++) {
		bufputc(buf, '-');
		bufputc(buf, todof->notes[line_index]->note[i]);
	}
	todof->notes[line_index]->note = buf->data;

	print_todo(todof);
	write_todo(todof);
}

static CMD check = {
	.use   = "cross <line>",
	.descr = "Cross items off the list",
	.run   = run_check,
};

static void run_get(CMD* cmd, int argc, char** args) {
	if (argc > 1) {
		error("Error: too many arguments");
	} else if (argc < 1) {
		error("Error: give line number");
	}

	TODO* todof = open_todo("./TODO", "r");
	int   index = parse_int(args[0]);

	if (index <= 0) {
		error("Error: please give a line number");
	} else if (index > todof->lines) {
		error("Error: todo list isn't that long");
	}

	printf("line %d:\n", index);
	printf("\t%s\n", todof->notes[index - 1]->note);
	close_todo(&todof);
}

static CMD get = {
	.use   = "get   <line>",
	.descr = "Show the <n>th item on the list",
	.run   = run_get,
};

static void run_delete(CMD* cmd, int argc, char** args) {
	for (int i = 0; i < argc; i++) {
		free(args[i]);
	}

	if (remove("./TODO") == 0) {
		printf("deleted TODO.\n");
	} else {
		printf("cannot delete 'TODO' file\n");
	}
}

static CMD del = {
	.use   = "del",
	.descr = "Delete the current TODO file from the disc",
	.run   = run_delete,
};

static void run_test(CMD* cmd, int argc, char** argv) {
	printf("stop. you shouldn't be using this!\n");
}

static CMD test = {
	.use    = "test",
	.descr  = "developer testing option",
	.run    = run_test,
	.hidden = true,
};

CMD todo = {
	.use   = "todo [flags] [option]",
	.descr = "Manage your todo list from the command prompt.\n\n"
	         "    Todo will take any arguments you give it and\n"
	         "    write them to a the TODO file as a list item\n"
	         "    as long as the first word does not match one of\n"
	         "    the commands. If this is the case just surround\n"
	         "    the note with quotes.",
	.run   = add_note,
};

Flag todo_flag = {
	.name      = "test",
	.descr     = "test out the todo command.",
	.shorthand = 't',
	.is_bool   = true,
	.hidden    = true,
};

Flag fileFlag = {
	.name      = "file",
	.shorthand = 'f',
	.descr     = "give the program a spesific file to open",
	.is_string = true,
};

static void init() {
	addFlag(&todo, todo_flag);
	addFlag(&todo, fileFlag);
	setRoot(&todo, false);

	addCommand(&del);
	addCommand(&check);
	addCommand(&get);
	addCommand(&rm);

	// hidden
	addCommand(&test);
}

int main(int argc, char* argv[]) {
	init();

	parse_opts(argc, argv);
	return close_cli();
}
