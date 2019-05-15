#ifndef FLAG_H
#define FLAG_H

#define MAX_FLAGS 10

struct command;

typedef struct flag {
	char*  name;
	char   shorthand;
	char*  descr;
	void*  value;
	size_t size; // size of value
	int    hidden;
	int    is_bool;
	int    is_string;
	int    triggered;
} Flag;

void addFlag(struct command* cmd, Flag* flag);
Flag* getFlag(struct command* cmd, char* name);
int is_flag(Flag* flag, char* arg);

void set_flag_value(Flag* flag, void* value);
void print_flags(struct command* cmd);
int maxOfFlags(int n, Flag** flags);
char* clean_flag_name(char* raw_arg);
int flag_count(int argc, char** argv);

#endif
