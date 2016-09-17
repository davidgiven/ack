#ifndef PUSH_POP_H
#define PUSH_POP_H

struct stackop {
	bool push : 1;
	char type : 7;
};

extern const struct stackop* stackops[];

#endif

