/** @file
 *  Program counter manipulation routines */

/* $Id$ */

#include	"em_abs.h"
#include	"global.h"
#include	"alloc.h"
#include	"trap.h"
#include	"text.h"
#include	"read.h"
#include	"proctab.h"
#include	"warn.h"

void init_text(void)
{
	DB = i2p(NTEXT);	/* set Descriptor Base */
	NProc = NPROC;		/* set Number of Proc. Descriptors */
	PI = -1;			/* initialize Procedure Identifier */
	PC = 0;				/* initialize Program Counter */

	text = Malloc((size)p2i(DB), "text space");
}


/************************************************************************
 *	Program Counter division					*
 ************************************************************************
 *									*
 *	newPC(p)	- check and adjust PC.				*
 *									*
 ************************************************************************/

void newPC(register ptr p)
{
	register struct proc *pr = &proctab[PI];

	if (p >= DB) {
		wtrap(WPCOVFL, EBADPC);
	}
	if (p < pr->pr_ep || p >= pr->pr_ff) {
		wtrap(WPCPROC, EBADPC);
	}
	PC = p;
}

