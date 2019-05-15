#ifndef IO_H
#define IO_H

typedef struct _buffer {
	size_t size;
	size_t len;
	int    _growby;
	char*  buffer;
} Buffer;

char* spaces(int n);
Buffer* new_buffer();

void delete_buffer(Buffer*);

void bufputc(Buffer*, char);
int bufputs(Buffer*, const char*);

void growb(Buffer*, int);
int freadb(Buffer*, FILE*);

#endif
