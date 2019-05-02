#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp, strcpy, strlen

#include "command.h"

CMD _top;
CMD *_commands[_len];

char** getSlice(int arrlen, char** arr, int start) {
	// start is an index... hence the 'start - 1'
	char** newarr = malloc((arrlen - start - 1) * sizeof(char*));
	for (int i = start; i < arrlen; i++) {
		newarr[i - start] = malloc(strlen(arr[i]) * sizeof(char));
		newarr[i - start] = arr[i];
	}
	// remember to deallocate this memory
	return newarr;
}

void setRoot(CMD *top) {
	_top = *top;
}

int parse(int len, char **args) {
	for (int i = 1; i < len; i++) {
		if ((strcmp("help", args[i]) == 0) || (strcmp("--help", args[i]) == 0)) {
			help(_top);
			return 1;
		}

		CMD *current = findCommand(args[i]);
		if (current == NULL) {
			continue;
		}
		if ((len - i - 1) == 0 && current->hasargs) {
			printf("Error: no arguments to %s\n", current->use);
			return 1;
		}

		char** cmd_args = getSlice(len, args, i + 1);
		(*current).run(current, len - i - 1, cmd_args);
		free(cmd_args);
		return 1;
	}
	// help(_top);
	return 0;
}

int maxOf(int n_strs, char** strs) {
	int max = 0, l;
	for (int i = 0; i < n_strs; i++) {
		l = strlen(strs[i]);
		if (l > max)
			max = l;
	}
	return max;
}

void help(CMD top) {
	int i = 0;
	CMD *arg;
	printf("Use:\n  %s [option]\n\n", top.use);
	printf("Option:\n");

	i = 0;
	while (1) {
		arg = _commands[i];
		if (arg == NULL) {
			printf("  help    get help on a command\n");
			return;
		}
		printf("  %s    %s\n", arg->use, arg->descr);
		i++;
	}
}

void addCommand(CMD *cmd) {
	for (int i = 0; i < _len; i++) {
		if (_commands[i] == NULL) {
			_commands[i] = cmd;
			return;
		}
	}
}

static char* get_cmd_name(char* usage) {
	int i, n = 0;
	int length = strlen(usage);

	for (i = 0; i < length+1; i++) {
		if (usage[i] == ' ' || usage[i] == '\0') {
			n = i;
			break;
		}
	}

	char* name = malloc(n + 1);
	for (i = 0; i < n; i++)
		name[i] = usage[i];

	name[i] = '\0';
	return name;
}

CMD *findCommand(char *name) {
	for (int i = 0; i < _len; i++) {
		if (_commands[i] == NULL) {
			return NULL;
		}

		char* cname = get_cmd_name(_commands[i]->use);
		// printf("find this: %s\n", cname);
		if (strcmp(cname, name) == 0) {
			free(cname);
			return _commands[i];
		}
		free(cname);
	}
	return NULL;
}
