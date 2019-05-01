#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"

CMD _top;
CMD *_commands[_len];

char** getSlice(int arrlen, char** arr, int start) {
	// start is an index... hence the 'start - 1'
	char** newarr = malloc((arrlen - start - 1) * sizeof(char*));
	for (int i = start; i < arrlen; i++) {
		newarr[i - start] = malloc(sizeof(arr[i]) * sizeof(char));
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
		if (strcmp("--help", args[i]) == 0 || strcmp("help", args[i]) == 0) {
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

void help(CMD top) {
	int i = 0;
	int max = 4;
	CMD *arg;
	printf("Use:\n  %s [command]\n\n", top.use);
	printf("Commands:\n");
	while (1) {
		arg = _commands[i];
		if (arg == NULL)
			break;

		int l = sizeof(arg->use);
		if (l > max)
			max = l;
		i++;
	}

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

CMD *findCommand(char *name) {
	for (int i = 0; i < _len; i++) {
		if (_commands[i] == NULL) {
			return NULL;
		}
		if (strcmp(_commands[i]->use, name) == 0) {
			return _commands[i];
		}
	}
	return NULL;
}
