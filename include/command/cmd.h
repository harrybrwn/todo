#ifndef COMMAND_H
#define COMMAND_H

#ifndef true
#  define true 1
#endif

#ifndef false
#  define false 0
#endif

#include "util/map.h"
#include "command/flag.h"

typedef void (* run_func)(struct command* cmd, int argc, char** argv);

typedef struct command {
	char*    use;
	char*    descr;
	int      hidden;
	Flag*    flags;
	int      n_flags;
	Map*     flag_map;
	run_func run;

	struct
	command** _sub_cmds;
	char*     _cmd_name;
	int       _n_cmds;
} CMD;

int parse_opts(int, char**);
void setRoot(CMD*, int);

void addCommand(CMD*);
void addToCommand(CMD*, CMD*);

void help();
int close_cli();

#endif
