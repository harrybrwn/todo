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

	TODO *todo = malloc(sizeof(TODO));
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

void close_note(Note** n) {
	if (*n == NULL)
		return;

	// if ((*n)->category != NULL)
	// 	free((*n)->category);


	free((*n)->note);
	(*n)->note = NULL;

	free(*n);
	(*n) = NULL;
}

void close_todo(TODO **todof) {
	fclose((*todof)->stream);
	for (int i = 0; i < (*todof)->lines; i++) {
		if ((*todof)->notes[i] != NULL) {
			close_note(&((*todof)->notes[i]));
		}
	}
	free((*todof)->notes);

	free(*todof);
	(*todof) = NULL;
}

void write_note(Note *note, FILE *file) {
	fprintf(file, "%d. %s\n", note->line, note->note);
}

void write_todo(TODO *todo) {
	for (int i = 0; i < todo->lines; i++) {
		if (todo->notes[i] == NULL)
			continue;
		write_note(todo->notes[i], todo->stream);
	}
}

Note* new_note(int lineno, char* data, char* category) {
	Note *n = malloc(sizeof(Note));

	n->line = lineno;
	n->note = data;
	n->category = category;
	return n;
}

Note* read_note(FILE *file) {
	Note *note = malloc(sizeof(Note));
	char num[3];

	char c;
	for (int i = 0; (c = fgetc(file)) != '.'; i++) {
		if (c == EOF)
			return NULL;

		if (isInt(c)) {
			num[i] = c;
		}
	}
	while ((c = fgetc(file)) != ' ') {}; // skip white space between line no. and note

	// line number
	char *end;
	note->line = strtoumax(num, &end, 10);

	note->length = 0;
	int buf_size = 16;
	note->note = malloc(buf_size);

	while (c != EOF) {
		c = fgetc(file);
		if (c == '\n')
			break;

		if (note->length == buf_size) {
			buf_size += 16;
			note->note = realloc(note->note, buf_size * sizeof(char));
		}

		note->note[note->length++] = c;
	}

	return note;
}
