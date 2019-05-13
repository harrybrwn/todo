#ifndef FLAG_H
#define FLAG_H

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

	char* __name;
	char  __shorthand[2];
} Flag;

void addFlag(struct command* cmd, Flag flag);
void set_flag_value(Flag* flag, void* value);
int maxOfFlags(int n, Flag* flags);

#endif
