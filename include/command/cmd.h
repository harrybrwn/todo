#ifndef COMMAND_H
#define COMMAND_H

#define BUG(msg) printf("%s:%d - %s(): \"%s\"\n", __FILE__, __LINE__, __FUNCTION__, msg)

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
	char* use;
	char* descr;
	int   hidden;
	Map*     flag_map;
	run_func run;

	struct
	command** _sub_cmds;
	char*     _cmd_name;
	int       _n_cmds;
	char**    _flag_names;
	int       _n_flags;
} CMD;

int parse_opts(int, char**);
void setRoot(CMD*, int);

void addCommand(CMD* cmd);
void addToCommand(CMD* root, CMD* cmd);

void help();
int close_cli();

#endif
