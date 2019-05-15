#include <stdlib.h>
#include "util/stack.h"

void push_arg(Args** stk, char* arg) {
	Args* new = malloc(sizeof(Args));
	new->val = arg;
	new->next = *stk;
	*stk = new;
}

char* pop_arg(Args** stk) {
	struct string_arg* tmp = *stk;
	*stk = (*stk)->next;
	tmp->next = NULL;
	char* data = tmp->val;
	free(tmp);
	return data;
}

int hasnext(Args** stk) {
	if (stk == NULL) {
		return 0;
	}
	return 1;
}

Args* rev_stack(int argc, char** argv) {
	if (argc == 0) {
		return NULL;
	}
	Args*                   stk = malloc(sizeof(Args));
	stk->val = argv[0]; int start = 1;
	stk->next = NULL;

	for (int i = start; i < argc; i++) {
		push_arg(&stk, argv[i]);
	}
	return stk;
}

Args* arg_stack(int argc, char** argv) {
	if (argc == 0) {
		return NULL;
	}
	Args* stk2 = malloc(sizeof(Args));
	stk2->next = NULL;
	stk2->val = argv[--argc];
	while (argc > 0) {
		push_arg(&stk2, argv[--argc]);
	}
	return stk2;
}
