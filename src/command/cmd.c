#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp, strcpy, strlen

#include "util/io.h"
#include "util/map.h"
#include "util/stack.h"
#include "command/cmd.h"
#include "command/flag.h"

static CMD* _root = NULL;
static int  _override_usage = 0;

static void usage();
static void init_cmd(CMD*);
static void close_cmd(CMD*);

static CMD* find_command(CMD** cmds, int len, char* name) {
	for (int i = 0; i < len; i++) {
		if (cmds[i] == NULL) {
			continue;
		}

		if (strcmp(cmds[i]->_cmd_name, name) == 0) {
			return cmds[i];
		}
	}
	return NULL;
}

static CMD* findCommand(char* name) {
	return find_command(_root->_sub_cmds, _root->_n_cmds, name);
}

static void run_root(CMD* root, int argc, char** argv) {
	usage(*root);
}

void setRoot(CMD* cmd, int override_usage) {
	if (cmd->run == NULL) {
		cmd->run = run_root;
	}
	init_cmd(cmd);
	_root = cmd;
	_override_usage = override_usage;
}

static int isHelp(char* name) {
	if (
		strcmp("help", name) == 0   ||
		strcmp("--help", name) == 0 ||
		strcmp("-h", name) == 0
		) {
		return 1;
	}
	return 0;
}

void help() {
	usage(*_root);
}

void printarr(int c, char** arr) {
	printf("[");
	for (int i = 0; i < c; i++) {
		if (i == c - 1) {
			printf("%s", arr[i]);
		} else {
			printf("%s, ", arr[i]);
		}
	}
	printf("]\n");
}

typedef struct cmd_args {
	int    argc;
	char** argv;
	int    flagcount;
} CmdArgs;

static int trigger_flag(CMD* cmd, char* name, Args** args) {
	int args_used = 0;
	for (int i = 0; i < cmd->_nflags; i++) {
		if (is_flag(cmd->_flags[i], name)) {
			cmd->_flags[i]->triggered = 1;
			if (hasnext(args)) {
				args_used++;
				set_flag_value(cmd->_flags[i], pop_arg(args));
			} else {
				set_flag_value(cmd->_flags[i], NULL);
			}
		}
	}
	return args_used;
}

static struct cmd_args* parse_flags(CMD* cmd, int argc, char** argv) {
	int    fcount = flag_count(argc, argv);
	int    n_args = argc - fcount;
	int    cnt = 0;
	char** newargs = malloc(n_args * sizeof(char*));
	Args*  args = arg_stack(argc, argv);

	char* tmp;
	while (args != NULL) {
		tmp = pop_arg(&args);
		if (tmp[0] == '-') {
			n_args -= trigger_flag(cmd, tmp, &args);
		} else {
			newargs[cnt++] = tmp;
		}
	}

	struct cmd_args* new = malloc(sizeof(CmdArgs));
	new->argc = cnt;
	new->argv = newargs;
	new->flagcount = fcount;
	return new;
}

static void run_cmd(CMD* cmd, int argc, char** argv) {
	CmdArgs* args = parse_flags(cmd, argc, argv);

	(*cmd).run(cmd, args->argc, args->argv);
	free(args->argv);
	free(args);
}

int parse_opts(int argc, char** argv) {
	CMD* cmd;
	if (_root == NULL) {
		fprintf(stderr, "Error: must set root command.\n");
		exit(1);
	}

	if (argc == 1) {
		run_cmd(_root, --argc, ++argv);
		return false;
	}

	if (isHelp(argv[1])) {
		if (argc == 2) {
			usage(*_root);
			return true;
		} else if (argc == 3) {
			cmd = findCommand(argv[2]);
			if (cmd == NULL) {
				fprintf(stderr, "could not find '%s'\n", argv[2]);
				return false;
			}
			usage(*cmd);
			return true;
		} else {
			return false;
		}
	}
	argc--;
	cmd = findCommand((++argv)[0]);

	if (cmd == NULL) {
		cmd = _root;
	} else {
		argc--;
		argv++;
	}
	run_cmd(cmd, argc, argv);
	return true;
}

int close_cli() {
	for (int i = 0; i < _root->_n_cmds; i++) {
		close_cmd(_root->_sub_cmds[i]);
	}
	free(_root->_sub_cmds);
	close_cmd(_root);
	return 0;
}

static int maxOfCMD(int n, CMD** cmds) {
	int max = 0, l;
	for (int i = 0; i < n; i++) {
		l = strlen(cmds[i]->use);
		if (l > max) {
			max = l;
		}
	}
	return max;
}

static const
char* help_template = "%s\n\n"
                      "Usage:\n  %s\n";

static void _print_cmd(CMD* c, char* spacer, int indent) {
	printf("  %s %.*s %s\n",
	       c->use,
	       (int)(indent - strlen(c->use)), spacer,
	       c->descr);
}

void cmd_usage(CMD cmd) {
	int   max = maxOfCMD(cmd._n_cmds, cmd._sub_cmds) + 1;
	char* spacer = spaces(max + 1);

	printf(help_template, cmd.descr, cmd.use);
	if (cmd._n_cmds > 0) {
		printf("\nOptions:\n");
		for (int i = 0; i < cmd._n_cmds; i++) {
			if (cmd._sub_cmds[i]->hidden) {
				continue;
			}
			_print_cmd(cmd._sub_cmds[i], spacer, max);
		}
	}

	free(spacer);
	printf("\nFlags:\n");
	if (cmd._nflags > 0) {
		print_flags(&cmd);
	}
	printf("  -h, --help   get help for %s\n", cmd._cmd_name);
}

static void usage(CMD top, int argc, char** argv) {
	if (_override_usage) {
		printf("%s\n", top.descr);
		return;
	}

	cmd_usage(top);
}

static char* get_cmd_name(char* usage) {
	int i = 0, n = 0;
	while (usage[i] != ' ' && usage[i] != '\0') {
		i++; n++;
	}

	char* name = malloc(n + 1);
	for (i = 0; i < n; i++) {
		name[i] = usage[i];
	}

	name[i] = '\0';
	return name;
}

static void init_cmd(CMD* cmd) {
	cmd->_cmd_name = get_cmd_name(cmd->use);
	cmd->_nflags = 0;
}

static void close_cmd(CMD* cmd) {
	free(cmd->_cmd_name);
}

void addToCommand(CMD* root, CMD* cmd) {
	if (root->_n_cmds == 0) {
		root->_sub_cmds = malloc(sizeof(CMD));
	} else {
		root->_sub_cmds = realloc(root->_sub_cmds, (root->_n_cmds + 1) * sizeof(CMD));
	}

	root->_sub_cmds[root->_n_cmds++] = cmd;
}

void addCommand(CMD* cmd) {
	if (_root == NULL) {
		printf("Error: must set root command.\n");
		exit(1);
	}
	init_cmd(cmd);
	addToCommand(_root, cmd);
}
