#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/io.h"
#include "command/cmd.h"
#include "command/flag.h"

static void flagtype_check(Flag f) {
	if (f.is_bool && f.is_string) {
		fprintf(stderr, "cannot have a flag that is both bool and string\n");
		exit(1);
	}
	if (f.is_bool && f.size != 0) {
		fprintf(stderr, "size of flag value does not match the type of the flag (int)\n");
		exit(1);
	}
	if (f.is_string && f.size != 0) {
		fprintf(stderr, "string flags are dynamically allocated, size should not be specified\n");
		exit(1);
	}
}

int maxOfFlags(int n, Flag** flags) {
	int max = 0, len;

	for (int i = 0; i < n; i++) {
		if (flags[i] == NULL) {
			continue;
		}
		len = strlen(flags[i]->name);
		if (len > max) {
			max = len;
		}
	}
	return max;
}

char* char_to_str(char c) {
	char* str = malloc(2);
	str[0] = c;
	str[1] = '\0';
	return str;
}

void addFlag(CMD* cmd, Flag* flag) {
	flagtype_check(*flag);

	if (cmd->_nflags == MAX_FLAGS - 1) {
		fprintf(stdout, "too many flags added to %s\n", cmd->_cmd_name);
		exit(1);
	}

	cmd->_flags[cmd->_nflags++] = flag;
}

Flag* getFlag(CMD* cmd, char* flagname) {
	for (int i = 0; i < cmd->_nflags; i++) {
		if (strcmp(flagname, cmd->_flags[i]->name) == 0) {
			return cmd->_flags[i];
		}
	}
	return (Flag*)NULL;
}

static int is_flag_name(char* name) {
	return name[0] == '-';
}

int flag_count(int argc, char** argv) {
	int c = 0;
	for (int i = 0; i < argc; i++) {
		if (is_flag_name(argv[i])) {
			c++;
		}
	}
	return c;
}

static void _print_flag(Flag* f, char* spacer, int indent) {
	if (f->shorthand == '\0') {
		printf("       --%s %.*s %s\n", f->name,
		       indent - (int)strlen(f->name), spacer, f->descr);
	} else {
		printf("  -%c, --%s %.*s %s\n", f->shorthand, f->name,
		       indent - (int)strlen(f->name), spacer, f->descr);
	}
}

void print_flags(CMD* cmd) {
	int   max = maxOfFlags(cmd->_nflags, cmd->_flags);
	char* buf = spaces(max + 1);

	for (int i = 0; i < cmd->_nflags; i++) {
		if (cmd->_flags[i] != NULL && !cmd->_flags[i]->hidden) {
			_print_flag(cmd->_flags[i], buf, max);
		}
	}
	free(buf);
}

void set_flag_value(Flag* flag, void* value) {
	if (flag->is_string) {
		if (value == NULL) {
			printf("must give positional argument to %s\n", flag->name);
			exit(1);
		}
		flag->value = value;
	}
}

char* clean_flag_name(char* raw_arg) {
	char* cleaned;
	int   len = strlen(raw_arg);
	if (raw_arg[1] == '-') {
		cleaned = malloc(len - 1);
	} else {
		cleaned = malloc(2);
		len = 1;
	}
	int cnt = 0;
	for (int i = 0; i < len; i++) {
		if (raw_arg[i] == '-') {
			continue;
		}
		cleaned[cnt++] = raw_arg[i];
	}
	cleaned[cnt] = '\0';
	return cleaned;
}

int is_flag(Flag* flag, char* arg) {
	char* name = clean_flag_name(arg);
	if (strlen(name) == 1) {
		if (name[0] == flag->shorthand) {
			free(name);
			return 1;
		}
	} else if (strcmp(name, flag->name) == 0) {
		free(name);
		return 1;
	}
	free(name);

	return 0;
}
