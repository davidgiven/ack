/*
	Handling the proctable
*/
#ifndef PROCTAB_H_
#define PROCTAB_H_

/* $Id$ */

#include	"global.h"

struct proc {
	size pr_nloc;
	ptr pr_ep;
	ptr pr_ff;			/* first address not in proc */
};

extern struct proc *proctab;

void init_proctab(void);
void add_proc(size, ptr);
void end_init_proctab(void);

#endif /* PROCTAB_H_ */
