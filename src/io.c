#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "io.h"

Buffer* new_buffer() {
	Buffer* b = malloc(sizeof(Buffer));
	b->len = 0;
	b->size = 32;
	b->_growby = 16;
	b->data = malloc(b->size * sizeof(char));
	return b;
}

void close_buffer(Buffer* buf) {
	free(buf->data);
	free(buf);
}

void bufputc(Buffer* buf, char c) {
	if (buf->len > buf->size)
		growb(buf, buf->_growby);

	buf->data[buf->len++] = c;
}

int putb(Buffer* buf, char* raw) {
	while (1) {
		if (raw[buf->len] == '\0')
			break;
		if (buf->len > buf->size)
			growb(buf, 16);

		bufputc(buf, raw[buf->len]);
	}
	return 1;
}

int freadb(Buffer* buf, FILE* fp) {
	char c;
	while ((c = fgetc(fp)) != EOF) {
		if (++buf->len > buf->size)
			growb(buf, 16);

		buf->data[buf->len] = c;
	}
	return 1;
}

void growb(Buffer* buf, int by) {
	buf->size += by;
	buf->data = realloc(buf->data, buf->size);
}
