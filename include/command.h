#ifndef COMMAND_H
#define COMMAND_H

#define _len 24

typedef struct command {
	char *use;
	char *descr;
	void (*run)(struct command *cmd, int, char** args);
	int hasargs;
	int hidden;
} CMD;

void setRoot(CMD*);
void addCommand(CMD*);
CMD *findCommand(char*);
int parse(int, char**);
char** getSlice(int, char**, int);
void help();
#endif
