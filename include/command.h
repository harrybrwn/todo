#ifndef COMMAND_H
#define COMMAND_H

#define _len 24

typedef struct command {
	char *use;
	char *descr;
	void (*run)(struct command *cmd, int, char** args);
	int hasargs;
} CMD;

void setToplevel(CMD*);
void addCommand(CMD*);
CMD *findCommand(char*);
int parse(int, char**);
void help(CMD);

#endif
