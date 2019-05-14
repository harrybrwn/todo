#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/io.h"
#include "util/map.h"
#include "command/cmd.h"
#include "command/flag.h"

#define MAX_FLAG_LENGTH 32

static void init_flag(Flag* f) {
	// f->__name = calloc(strlen(f->name) + 3, sizeof(char));
	// f->__name[0] = '-';
	// f->__name[1] = '-';
	// strcat(f->__name, f->name);

	// f->__shorthand[0] = '-';
	// f->__shorthand[1] = f->shorthand;
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

char* char_to_str(char c) {
	char* str = malloc(2);
	str[0] = c;
	str[1] = '\0';
	return str;
}

void addFlag(CMD* cmd, Flag* flag) {
	// cmd->flags = realloc(cmd->flags, (cmd->n_flags + 1) * sizeof(Flag));
	init_flag(flag);
	flagtype_check(*flag);
	// cmd->flags[cmd->n_flags++] = flag;
	cmd->_flag_names = realloc(cmd->_flag_names,
	                           MAX_FLAG_LENGTH * (cmd->_n_flags + 1));
	cmd->_flag_names[cmd->_n_flags++] = flag->name;

	if (cmd->flag_map == NULL) {
		cmd->flag_map = new_map();
	}

	put(cmd->flag_map, flag->name, &flag);
	if (flag->shorthand != '\0') {
		char* name = malloc(2);
		name[0] = flag->shorthand; name[1] = '\0';
		put(cmd->flag_map, name, flag->name);
	}
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

void print_flags(Map* flagm, char** names, int n) {
	// Flag** flags = malloc(n * sizeof(Flag*));
	int    max = 0, len;
	// for (int i = 0; i < n; i++) {
	// 	len = strlen(names[i]);
	// 	if (len > max) {
	// 		max = len;
	// 	}
	// 	flags[i] = get(flagm, names[i]);
	// }
	for (int i = 0; i < flagm->length; i++) {
		len = strlen(flagm->keys[i]);
		if (len > max)
			max = len;
	}
	char* spacer = spaces(max + 1);
	Flag* f;
	for (int i = 0; i <flagm->length; i++) {
		f = (Flag*)get(flagm, flagm->keys[i]);
		if (f->hidden) {
			continue;
		}
		_print_flag(*f, spacer, max);
	}

	// for (int i = 0; i < n; i++) {
	// 	if (flags[i]->hidden) {
	// 		continue;
	// 	}
	// 	_print_flag(*flags[i], spacer, max);
	// }
	// free(flags);
	free(spacer);
}

Flag* getFlag(CMD* cmd, char* name) {
	char* fname;
	if (strlen(name) == 1) {
		fname = (char*)get(cmd->flag_map, name);
	} else {
		fname = name;
	}
	return *(Flag**)get(cmd->flag_map, fname);
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
