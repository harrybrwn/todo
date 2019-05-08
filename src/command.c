#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp, strcpy, strlen

#include "command.h"

// static int MAX_N_CMDS = _len;
static int  n_cmds = 0;
static CMD  _top;
static CMD* _commands[_len];

void setRoot(CMD* top) {
	_top = *top;
}

static void Usage(CMD);

void help() {
	Usage(_top);
}

static int isHelp(char* name) {
	if ((strcmp("help", name) == 0) || (strcmp("--help", name) == 0)) {
		return 1;
	}
	return 0;
}

int parse_opts(int argc, char** argv) {
	if (argc == 1) {
		return false;
	}

	if (isHelp(argv[1])) {
		Usage(_top);
		return true;
	}

	CMD* cmd = findCommand((++argv)[0]);

	if (cmd == NULL) {
		return false;
	} else {
		argc--;
	}

	(*cmd).run(cmd, --argc, ++argv);
	return true;
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

static char* spaces(int n) {
	char* s = malloc(n);

	memset(s, ' ', n);
	return s;
}

static void Usage(CMD top) {
	printf("Use:\n  %s [option]\n\n", top.use);
	printf("Options:\n");

	int   max = maxOfCMD(n_cmds, _commands);
	char* spacer = spaces(max + 1);

	CMD* arg;

	for (int i = 0; i < n_cmds; i++) {
		arg = _commands[i];
		if (arg->hidden) {
			continue;
		}

		printf("  %s %.*s %s\n", arg->use, (int)(max - strlen(arg->use)), spacer, arg->descr);
	}
	printf("\nFlags:\n");
	printf("  --help   get help on a command\n");
	free(spacer);
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

void addCommand(CMD* cmd) {
	for (int i = 0; i < _len; i++) {
		// find the first empty CMD
		if (_commands[i] == NULL) {
			init_cmd(cmd);
			_commands[i] = cmd;
			n_cmds++;
			return;
		}
	}
}

CMD* findCommand(char* name) {
	for (int i = 0; i < _len; i++) {
		if (_commands[i] == NULL) {
			return NULL;
		}

		// char* cname = get_cmd_name(_commands[i]->use);
		if (strcmp(_commands[i]->_cmd_name, name) == 0) {
			// free(cname);
			return _commands[i];
		}
		// free(cname);
	}
	return NULL;
}
