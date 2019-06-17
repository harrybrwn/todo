#include <stdio.h>  // printf
#include <stdlib.h>  // malloc, calloc, realloc
#include <string.h> // strcmp, strcpy, strlen
#include <ctype.h> // isdigit
#include <inttypes.h>

#include "util/fileio.h"  // file_len, file_lines
#include "util/io.h"
#include "todo.h"

TODO* open_todo(const char* fname, const char* mode) {
	FILE* f = fopen(fname, mode);

	if (f == NULL) {
		FILE* newf = fopen(fname, "w+");

		fclose(newf);
		f = fopen(fname, mode);
	}

	TODO* todo = malloc(sizeof(TODO));

	todo->filename = fname;
	todo->stream = f;

	if (
		(strcmp("w", mode) == 0)  ||
		(strcmp("w+", mode) == 0) ||
		(strcmp("a", mode) == 0)
		) {
		return todo;
	}

	FileInfo* info = get_info(f);

	todo->length = info->length;
	todo->lines = info->lines;
	free(info);

	todo->notes = malloc(sizeof(Note*) * todo->lines);
	for (int i = 0; i < todo->lines; i++) {
		todo->notes[i] = read_note(todo->stream);
	}

	return todo;
}

void print_todo(TODO* todo) {
	if (todo->length == 1) {
		printf("your TODO is empty\n");
		return;
	}

	int count = 0;

	for (int i = 0; i < todo->lines; i++) {
		if (todo->notes[i] == NULL) {
			continue;
		}

		printf("%d. %s\n", ++count, todo->notes[i]->note);
	}
}

void close_note(Note** n) {
	if (*n == NULL) {
		return;
	}

	if ((*n)->note != NULL) {
		free((*n)->note);
		(*n)->note = NULL;
	}
	free(*n);
	(*n) = NULL;
}

void close_todo(TODO** todof) {
	fclose((*todof)->stream);
	for (int i = 0; i < (*todof)->lines; i++) {
		if ((*todof)->notes[i] != NULL) {
			close_note(&((*todof)->notes[i]));
			(*todof)->notes[i] = NULL;
		}
	}
	free((*todof)->notes);

	if (*todof != NULL) {
		free(*todof);
		(*todof) = NULL;
	}
}

void write_note(Note* note, FILE* file) {
	fprintf(file, "%d. %s\n", note->line, note->note);
}

void write_todo(TODO* todo) {
	FILE* fp = fopen(todo->filename, "w+");

	int count = 0;

	for (int i = 0; i < todo->lines; i++) {
		if (todo->notes[i] == NULL) {
			continue;
		}
		todo->notes[i]->line = ++count;
		write_note(todo->notes[i], fp);
	}
	fclose(fp);
}

Note* new_note(int lineno, char* data, char* category) {
	Note* n = malloc(sizeof(Note));

	n->line = lineno;
	n->note = data;
	return n;
}

Note* read_note(FILE* file) {
	Note* note = malloc(sizeof(Note));
	char  num[3];
	char  c;

	for (int i = 0; (c = fgetc(file)) != '.'; i++) {
		if (c == EOF) {
			return NULL;
		}
		if (isdigit(c)) {
			num[i] = c;
		}
	}
	while ((c = fgetc(file)) != ' ') {}; // skip white space between line no. and note

	// line number
	char* end;
	note->line = (int)strtoimax(num, &end, 10);

	note->length = 0;
	Buffer* buf = new_buffer(16);

	while (c != EOF) {
		c = fgetc(file);
		if (c == '\n') {
			break;
		}

		bufputc(buf, c);
	}
	bufputc(buf, '\0');
	note->note = buf->buffer;
	note->length = buf->len;
	free(buf);

	return note;
}
