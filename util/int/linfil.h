/*
	This file includes all (the arbitrary) details of the implementation
	of the present line number and file name in the EM machine.
	
	For efficiency reasons the EM machine keeps its own copies of the
	file name and the line number.
*/

/* $Id$ */

/* these should be EM machine registers */
extern long LIN;
extern ptr FIL;				/* address in data[] */

#define	getLIN()	(LIN)
#define	getFIL()	(FIL)

extern char *dt_fname();
extern char *position();

