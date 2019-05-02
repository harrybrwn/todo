#include <stdio.h>
#include <stdlib.h>

#include "fileio.h"

FileInfo* file_info(const char* filename) {
	FILE *fpt = fopen(filename, "r");
	if (fpt == NULL) {
		printf("cannot read file\n");
		exit(1);
	}

	FileInfo file_info = { .lines = 0, .length = 0 };
	char c;
	while (c != EOF) {
		c = fgetc(fpt);
		file_info.length++;
		if (c == '\n')
			file_info.lines++;
	}
	fclose(fpt);

	FileInfo *info_ptr = malloc(sizeof(FileInfo*));
	info_ptr = &file_info;
	return info_ptr;
}

int file_len(const char* filename) {
	FILE *fpt = fopen(filename, "r");
	if (fpt == NULL) {
		printf("cannot read file\n");
		exit(1);
	}

	int n_chars = 0;
	char c;
	while (c != EOF) {
		c = fgetc(fpt);
		n_chars++;
	}
	fclose(fpt);
	return n_chars;
}

int file_lines(const char* filename) {
	FILE *fpt = fopen(filename, "r");
	if (fpt == NULL) {
		printf("cannot read file\n");
		exit(1);
	}

	int n_lines = 0;
	char c;
	while (c != EOF) {
		c = fgetc(fpt);
		if (c == '\n') {
			n_lines++;
		}
	}
	fclose(fpt);
	return n_lines;
}

void print_nextline(FILE *stream) {
	char c = fgetc(stream);
	while (c != EOF) {
		printf("%c", c);
		c = fgetc(stream);
		if (c == '\n') {
			printf("%c", c);
			return;
		}
	}
}

void fseek_line(FILE *stream, int line) {
	fseek(stream, 0, 0);
	int current_line = 0;
	char c;
	while (current_line != line - 1) {
		c = fgetc(stream);
		if (c == '\n')
			++current_line;
	}
}
