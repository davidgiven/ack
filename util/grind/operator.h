/* $Id$ */

#include "ops.h"

typedef struct operator {
	int	op_nargs;
	int	(*op_fun)();
} t_operator, *p_operator;

extern t_operator operators[];

#define nargs(n)	(operators[(n)].op_nargs)
