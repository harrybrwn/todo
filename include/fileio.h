#ifndef FILEIO_H
#define FILEIO_H

typedef struct _fileinfo {
	int lines;
	int length;
} FileInfo;

int file_len(const char*);
int file_lines(const char*);
FileInfo* file_info(const char*);
void fseek_line(FILE*, int);
void print_nextline(FILE*);

#endif
