#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util/io.h"

char* spaces(int n) {
	char* s = malloc(n);

	memset(s, ' ', n);
	return s;
}

Buffer* new_buffer(int size) {
	Buffer* b = malloc(sizeof(Buffer));

	b->len = 0;
	b->size = size;
	b->_growby = 16;
	b->data = malloc(b->size * sizeof(char));
	return b;
}

void close_buffer(Buffer* buf) {
	free(buf->data);
	free(buf);
}

void bufputc(Buffer* buf, char c) {
	if (buf->len >= buf->size) {
		growb(buf, buf->_growby);
	}

	buf->data[buf->len++] = c;
}

int bufputs(Buffer* buf, const char* raw) {
	for (int i = 0; i < strlen(raw); i++) {
		if (raw[i] == '\0') {
			printf("yeah were good\n");
		}
		bufputc(buf, raw[i]);
	}
	return 1;
}

int freadb(Buffer* buf, FILE* fp) {
	char c;

	while ((c = fgetc(fp)) != EOF) {
		if (++buf->len > buf->size) {
			growb(buf, 16);
		}

		buf->data[buf->len] = c;
	}
	return 1;
}

void growb(Buffer* buf, int by) {
	buf->size += by;
	buf->data = realloc(buf->data, buf->size);
}
