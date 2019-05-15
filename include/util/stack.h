#ifndef STACK_H
#define STACK_H

typedef struct string_arg {
	char*              val;
	struct string_arg* next;
} Args;

void  push_arg(Args** stk, char* arg);
char* pop_arg(Args** stk);
int   hasnext(Args** stk);
Args* arg_stack(int argc, char** argv);
Args* rev_stack(int argc, char** argv);

#endif
