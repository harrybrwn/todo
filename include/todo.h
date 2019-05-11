#ifndef TODO_H
#define TODO_H

typedef struct _note {
	int   line;
	char* note;
	int   length;
} Note;

typedef struct {
	FILE*       stream;
	Note**      notes;
	char*       raw;
	int         length;
	int         lines;
	const char* filename;
} TODO;

TODO* open_todo(const char* e, const char*);

void write_todo(TODO*);
void print_todo(TODO*);
void close_todo(TODO**);

Note* new_note(int, char*, char*);

void write_note(Note*, FILE*);
Note* read_note(FILE*);

void close_note(Note**);

#endif
