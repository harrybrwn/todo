#ifndef TODO_H
#define TODO_H

typedef struct _note {
	int  line;
	char *note;
	int  length;
	char *category;
} Note;

typedef struct {
	FILE *stream;
	Note *notes;
	char *raw;
	int  length;
	int  lines;
} TODO;

TODO* open_todo(const char* fname, const char *mode);
void close_todo(TODO** todof);

#endif
