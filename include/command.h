#ifndef COMMAND_H
#define COMMAND_H

#define _len 24

#ifndef true
#  define true 1
#endif

#ifndef false
#  define false 0
#endif

typedef struct flag {
	char* name;
	void (*action)();
} Flag;

typedef struct command {
	char *use;
	char *descr;
	void (*run)(struct command *cmd, int, char** args);
	int hasargs;
	int hidden;

	char* _cmd_name;
	struct command** _sub_cmds;
} CMD;

int parse_opts(int, char**);
void setRoot(CMD*);
void addCommand(CMD*);
void addToCommand(CMD*, CMD*);

CMD *findCommand(char*);
void help();


#endif
