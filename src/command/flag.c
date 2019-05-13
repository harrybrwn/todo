#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command/cmd.h"
#include "command/flag.h"

static void init_flag(Flag* f) {
	f->__name = calloc(strlen(f->name) + 3, sizeof(char));
	f->__name[0] = '-';
	f->__name[1] = '-';
	strcat(f->__name, f->name);

	f->__shorthand[0] = '-';
	f->__shorthand[1] = f->shorthand;
}

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

int maxOfFlags(int n, Flag* flags) {
	int max = 0, len;

	for (int i = 0; i < n; i++) {
		len = strlen(flags[i].name);
		if (len > max) {
			max = len;
		}
	}
	return max;
}

void addFlag(CMD* cmd, Flag flag) {
	cmd->flags = realloc(cmd->flags, (cmd->n_flags + 1) * sizeof(Flag));
	init_flag(&flag);
	flagtype_check(flag);
	cmd->flags[cmd->n_flags++] = flag;
}

void set_flag_value(Flag* flag, void* value) {
	if (flag->is_string) {
		int size;
		if (value == NULL) {
			printf("must give positional argument to %s\n", flag->name);
			exit(1);
			size = 4;
		} else {
			size = strlen((char*)value);
		}
		flag->value = malloc(size);
		flag->value = value;
	}
}
