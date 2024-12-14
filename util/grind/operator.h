/* $Id$ */

#include "ops.h"

struct tree;
typedef struct operator
{
	int op_nargs;
	void (*op_fun)(struct tree*);
}
t_operator, *p_operator;

extern t_operator operators[];

#define nargs(n) (operators[(n)].op_nargs)
