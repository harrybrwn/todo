#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp, strcpy, strlen

#include "command.h"

static CMD* _root;
static int  _override_usage = 0;

static void usage();
static CMD* findCommand(char*);
static void close_cmd(CMD*);

static void root_use(CMD* root, int argc, char** argv) {
	usage(*root);
}

void setRoot(CMD* cmd, int override_usage) {
	if (cmd->run == NULL) {
		cmd->run = root_use;
	}
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

int parse_opts(int argc, char** argv) {
	if (_root == NULL) {
		printf("Error: must set root command.\n");
		exit(1);
	}

	if (argc == 1) {
		(*_root).run(_root, --argc, ++argv);
		return false;
	}

	if (isHelp(argv[1])) {
		usage(*_root);
		return true;
	}
	CMD* cmd = findCommand((++argv)[0]);

	if (cmd == NULL) {
		(*_root).run(_root, --argc, argv);
		return false;
	} else {
		argc--;
	}

	(*cmd).run(cmd, --argc, ++argv);
	return true;
}

void close_cli() {
	for (int i = 0; i < _root->_n_cmds; i++) {
		close_cmd(_root->_sub_cmds[i]);
	}
	free(_root->_sub_cmds);
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

static int maxOfFlags(int n, Flag* flags) {
	int max = 0, len;

	for (int i = 0; i < n; i++) {
		len = strlen(flags[i].name);
		if (len > max) {
			max = len;
		}
	}
	return max;
}

static char* spaces(int n) {
	char* s = malloc(n);

	memset(s, ' ', n);
	return s;
}

static const
char* help_template = "%s\n\n"
                      "Usage:\n  %s\n\n"
                      "Options:\n";

static void _print_cmd(CMD* c, char* spacer, int indent) {
	printf("  %s %.*s %s\n",
	       c->use,
	       (int)(indent - strlen(c->use)), spacer,
	       c->descr);
}

static void _print_flag(Flag f, char* spacer, int indent) {
	if (f.shorthand == '\0') {
		printf("       --%s %.*s %s\n", f.name,
		       indent - (int)strlen(f.name), spacer, f.descr);
	} else {
		printf("  -%c, --%s %.*s %s\n", f.shorthand, f.name,
		       indent - (int)strlen(f.name), spacer, f.descr);
	}
}

void cmd_usage(CMD cmd) {
	int   max = maxOfCMD(cmd._n_cmds, cmd._sub_cmds) + 1;
	char* spacer = spaces(max + 1);

	printf(help_template, cmd.descr, cmd.use);

	for (int i = 0; i < cmd._n_cmds; i++) {
		if (cmd._sub_cmds[i]->hidden) {
			continue;
		}
		_print_cmd(cmd._sub_cmds[i], spacer, max);
	}

	max = maxOfFlags(cmd.n_flags, cmd.flags) + 1;
	free(spacer);
	spacer = spaces(max);

	printf("\nFlags:\n");
	for (int i = 0; i < cmd.n_flags; i++) {
		if (cmd.flags[i].hidden) {
			continue;
		}
		_print_flag(cmd.flags[i], spacer, max);
	}
	free(spacer);
}

static void usage(CMD top) {
	if (_override_usage) {
		printf("%s\n", top.descr);
		return;
	}

	cmd_usage(top);
	printf("  -h, --help   get help for todo\n");
}

static char* get_cmd_name(char* usage) {
	int i, n = 0;
	int length = strlen(usage);

	for (i = 0; i < length + 1; i++) {
		if (usage[i] == ' ' || usage[i] == '\0') {
			n = i;
			break;
		}
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
	if (cmd->n_flags != 0) {
		perror("'n_flags' is a private field, do not touch it");
		exit(1);
	}
	init_cmd(cmd);
	addToCommand(_root, cmd);
}

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

static void flagtype_check(Flag f) {
	if (f.is_bool && f.is_string) {
		printf("cannot have a flag that is both bool and string\n");
		exit(1);
	}
	if (f.is_bool && f.size != 0) {
		printf("size of flag value does not match the type of the flag (int)\n");
		exit(1);
	}
	if (f.is_string && f.size != 0) {
		printf("string flags are dynamically allocated, size should not be specified\n");
		exit(1);
	}
}

void addFlags(CMD* cmd, Flag* flags, int nflags) {
	for (int i = 0; i < nflags; i++) {
		if (flags[i].triggered) {
			printf("cannot add a flag that has already been triggered\n");
			exit(1);
		}
		flagtype_check(flags[i]);
	}
	cmd->n_flags = nflags;
	cmd->flags = flags;
}

void addFlag(CMD* cmd, Flag flag) {
	cmd->flags = realloc(cmd->flags, (cmd->n_flags + 1) * sizeof(Flag));
	cmd->flags[cmd->n_flags++] = flag;
}
