/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "bem.h"

#ifndef NORSCID
static char rcs_id[] = "$Header$" ;
#endif

/* Split the intermediate code into procedures.
   This is necessary to make the EM code fit on
   smaller machines. (for the Peephole optimizer!)
*/

/* Heuristic is to collect all basic blocks of more then THRESHOLD
   em instructions into a procedure
*/

int	procnum;
int	threshold;	/* can be set by the user */


fix(lnr)
int lnr;
{
	/* this block may not be moved to a procedure */
	Linerecord *lr;

	if(debug) printf("fixate %d\n",lnr);
	for(lr= firstline;lr; lr=lr->nextline)
	if( lr->linenr == lnr)
		lr->fixed=1;
}

fixblock(l)
List *l;
{
	while(l)
	{
		fix(l->linenr);
		l=l->nextlist;
	}
}
phase1()
{
	/* copy all offloaded blocks */
	Linerecord	*lr, *lf;
	long		blksize;

	lf= lr= firstline;
	blksize= lr->codelines;
	while( lr)
	{
		if( lr->fixed){
			if( !lf->fixed && blksize>threshold)
			{
				/*move block */
				if(debug) printf("%d %d->%d moved\n",
					blksize,lf->linenr, lr->linenr);
			}
			lf= lr;
			blksize= lr->codelines;
		}
		lr= lr->nextline;
	}
}
phase2()
{
	/* copy main procedure */
	prolog2();
	epilogcode();
}
split()
{
	/* selectively copy the intermediate code to procedures */
	Linerecord	*lr;

	if( debug) printf("split EM code using %d\n",threshold);

	/* First consolidate the goto's and caller's */
	lr= firstline;
	while(lr)
	{
		fixblock(lr->callers);
		fixblock(lr->gotos);
		lr= lr->nextline;
	}

	/* Copy the temporary file piecewise */
	prologcode();
	phase1();
	phase2();
}
