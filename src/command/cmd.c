#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcmp, strcpy, strlen

#include "util/io.h"
#include "util/map.h"
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

int is_flag(char* flag) {
	if (flag[0] == '-') {
		return true;
	}
	return false;
}

static void run_cmd(CMD* cmd, int argc, char** argv) {
	int    argcount = 0;
	char** new_argv = malloc(argc * sizeof(char*));

	char* fkey;
	for (int i = 0; i < argc; i++) {
		if (argv[i][0] != '-') {
			new_argv[argcount++] = argv[i];
			continue;
		}
		BUG("found flag");
		printf("%s\n", argv[i]);

		if (is_flag(argv[i])) {
			char* flagname = clean_flag_name(argv[i]);
			if (strlen(flagname) == 1) {
				fkey = (char*)get(cmd->flag_map, flagname);
			} else {
				fkey = flagname;
			}
			Flag* f = getFlag(cmd, fkey);
			if (f == NULL) {
				printf("null flag\n");
			}

			f->triggered = 1;
			if (i == argc - 1) {
				set_flag_value(f, NULL);
			} else {
				set_flag_value(f, argv[++i]);
			}
			put(cmd->flag_map, fkey, &f);
			free(flagname);
		}
	}
	new_argv = realloc(new_argv, argcount * sizeof(char*));

	(*cmd).run(cmd, argcount, new_argv);
	free(new_argv);
}

int parse_opts(int argc, char** argv) {
	CMD* cmd;
	if (_root == NULL) {
		fprintf(stderr, "Error: must set root command.\n");
		exit(1);
	}

	if (argc == 1) {
		(*_root).run(_root, --argc, ++argv);
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
                      "Usage:\n  %s\n\n"
                      "Options:\n";

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

	for (int i = 0; i < cmd._n_cmds; i++) {
		if (cmd._sub_cmds[i]->hidden) {
			continue;
		}
		_print_cmd(cmd._sub_cmds[i], spacer, max);
	}

	free(spacer);
	// max = maxOfFlags(cmd.n_flags, cmd.flags) + 1;
	// spacer = spaces(max);

	printf("\nFlags:\n");
	// Flag* f;
	// for (int i = 0; i < cmd._n_flags; i++) {
	// 	f = (Flag*)get(cmd.flag_map, cmd._flag_names[i]);
	// 	if (f->hidden) {
	// 		continue;
	// 	}
	// 	_print_flag(*f, spacer, max);
	// }
	// free(spacer);
	// print_flags(cmd.flag_map, cmd._flag_names, cmd._n_flags);
	for (int i = 0; i < cmd.flag_map->length; i++) {
		printf("flagname: %s\n", cmd.flag_map->keys[i]);
	}
}

static void usage(CMD top) {
	if (_override_usage) {
		printf("%s\n", top.descr);
		return;
	}

	cmd_usage(top);
	printf("  -h, --help   get help for %s\n", top._cmd_name);
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
	if (cmd->flag_map == NULL) {
		cmd->flag_map = new_map();
	}
}

static void close_cmd(CMD* cmd) {
	free(cmd->_cmd_name);

	// Flag* f = NULL;
	// for (int i = 0; i < cmd->flag_map->length; i++) {
	// 	if (strlen(cmd->flag_map->keys[i]) == 1) {
	// 		free(cmd->flag_map->keys[i]);
	// 		continue;
	// 	}
	// 	// printf("getting '%s'\n", cmd->flag_map->keys[i]);
	// 	//
	// 	// if (cmd->flag_map->keys[i] == NULL)
	// 	// 	printf("wait this key is null\n");
	// 	// if (cmd->flag_map == NULL)
	// 	// 	printf("ok wtf guys\n");
	//
	// 	f = *(Flag**)get(cmd->flag_map, cmd->flag_map->keys[i]);
	// 	if (f == NULL) {
	// 		printf("%s was null in the map\n", cmd->flag_map->keys[i]);
	// 		continue;
	// 	}
	// 	printf("%p\n", f);
	// 	// if (f->name == NULL) {
	// 	// 	printf("what?\n");
	// 	// }
	// 	// printf("closing: '%s'\n", (*f).name);
	// }
	// close_map(cmd->flag_map);
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
	if (cmd->_n_flags != 0) {
		perror("'n_flags' is a private field, do not set it");
		exit(1);
	}
	init_cmd(cmd);
	addToCommand(_root, cmd);
}
