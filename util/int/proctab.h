/*
	Handling the proctable
*/

/* $Id$ */

struct proc {
	size pr_nloc;
	ptr pr_ep;
	ptr pr_ff;			/* first address not in proc */
};

extern struct proc *proctab;
