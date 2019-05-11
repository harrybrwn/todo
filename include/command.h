#ifndef COMMAND_H
#define COMMAND_H

#ifndef true
#  define true 1
#endif

#ifndef false
#  define false 0
#endif

typedef struct flag {
	char*  name;
	char   shorthand;
	char*  descr;
	void*  value;
	size_t size; // size of value
	int    hidden;
	int    is_bool;
	int    is_string;
	int    triggered;

	char* __name;
	char  __shorthand[2];
} Flag;

typedef struct command {
	char* use;
	char* descr;
	int   hidden;
	Flag* flags;
	int   n_flags;
	void (* run)(struct command* cmd, int, char** args);

	struct
	command** _sub_cmds;
	char*     _cmd_name;
	int       _n_cmds;
} CMD;

int parse_opts(int, char**);
void setRoot(CMD*, int);

void addCommand(CMD*);
void addFlags(CMD*, Flag*, int);
void addFlag(CMD*, Flag);

void addToCommand(CMD*, CMD*);

void help();
void close_cli();

#endif
