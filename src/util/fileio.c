#include <stdio.h>
#include <stdlib.h>

#include "util/fileio.h"

FileInfo* get_info(FILE* f) {
	fseek(f, 0, SEEK_SET);

	FileInfo* file_info = malloc(sizeof(FileInfo));
	char      c = '\0';

	file_info->length = 0;
	file_info->lines = 0;

	while (c != EOF) {
		c = fgetc(f);
		file_info->length++;
		if (c == '\n') {
			file_info->lines++;
		}
	}
	fseek(f, 0, SEEK_SET);
	return file_info;
}

FileInfo* file_info(const char* filename) {
	FILE* fpt = fopen(filename, "r");

	if (fpt == NULL) {
		printf("cannot read file\n");
		exit(1);
	}

	FileInfo* info_ptr = get_info(fpt);

	fclose(fpt);

	return info_ptr;
}

void close_info(FileInfo* info) {
	free(info);
}

int read(FILE* f, char** data) {
	int  i = 0;
	char c;

	while ((c = fgetc(f)) != EOF) {
		(*data)[i] = c;
	}
	return i;
}

int readuntil(FILE* f, int end, char** data) {
	int  i;
	char c;

	for (i = 0; ftell(f) != end; i++) {
		c = fgetc(f);
		if (c == EOF) {
			break;
		}

		(*data)[i] = c;
	}
	return i;
}

int readto_ch(FILE* f, char stp, char** data) {
	int  i;
	char c = fgetc(f);

	while (c != EOF || c != stp) {
		(*data)[i++] = c;
	}
	return i;
}

int read_range(FILE* f, int start, int end, char** data) {
	fseek(f, start, SEEK_SET);
	return readuntil(f, end, data);
}

int readall(FILE* f, char** data) {
	fseek(f, 0, SEEK_SET);
	return read(f, data);
}

void skipline(FILE* f) {
	char c;

	while ((c = fgetc(f)) != EOF) {
		if (c == '\n') {
			return;
		}
	}
}

int file_len(const char* filename) {
	FILE* fpt = fopen(filename, "r");

	if (fpt == NULL) {
		printf("cannot read file\n");
		exit(1);
	}

	int  n_chars = 0;
	char c;

	while (c != EOF) {
		c = fgetc(fpt);
		n_chars++;
	}
	fclose(fpt);
	return n_chars;
}

int file_lines(const char* filename) {
	FILE* fpt = fopen(filename, "r");

	if (fpt == NULL) {
		printf("cannot read file\n");
		exit(1);
	}

	int  n_lines = 0;
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

void print_nextline(FILE* stream) {
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

// move file pointer to the nth line (index starts at 0)
void fseek_line(FILE* stream, int line) {
	fseek(stream, 0, SEEK_SET);
	int  current_line = 0;
	char c;

	while (current_line != line) {
		c = fgetc(stream);
		if (c == '\n') {
			++current_line;
		}
	}
}
