#include <stdio.h>  // printf
#include <stdlib.h>  // malloc, calloc, realloc
#include <string.h> // strcmp, strcpy, strlen
#include <inttypes.h>

#include "fileio.h"  // file_len, file_lines
#include "todo.h"


static int isInt(char x) {
	switch (x) {
		case '0': return 1;
		case '1': return 1;
		case '2': return 1;
		case '3': return 1;
		case '4': return 1;
		case '5': return 1;
		case '6': return 1;
		case '7': return 1;
		case '8': return 1;
		case '9': return 1;
		default: return 0;
	}
}

TODO* open_todo(const char* fname, const char *mode) {
	FILE *f = fopen(fname, mode);
	if (f == NULL) {
		FILE *newf = fopen(fname, "w");
		fclose(newf);
		f = fopen(fname, mode);
	}

	TODO *todof = malloc(sizeof(TODO));
	todof->stream = f;

	if ((strcmp("r", mode) == 0) || (strcmp("r+", mode) == 0)) {
		FileInfo* info = get_info(f);
		todof->length = info->length;
		todof->lines = info->lines;
		free(info);
	}

	return todof;
}

void close_todo(TODO **todof) {
	fclose((*todof)->stream);
	free(*todof);
	(*todof) = NULL;
}

void write_note(Note *note, FILE *file) {
	fprintf(file, "%d. %s\n", note->line, note->note);
}

void write_todo(TODO *todo) {
	for (int i = 0; i < todo->lines; i++) {

	}
}

Note* read_note(FILE *file) {
	Note *note = malloc(sizeof(Note));
	char num[128];
	int i;

	char c;
	for (i = 0; (c = fgetc(file)) != '.'; i++) {
		if (isInt(c))
			num[i] = c;
	}

	char *end;
	note->line = strtoumax(num, &end, 10);
	note->length = 0;

	// while (c != EOF || c != '\n') {
	//
	// }

	for (i = 0; (c = fgetc(file)) != EOF; i++) {

	}
	return note;
}
