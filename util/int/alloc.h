/*
	Rather than using malloc and realloc, which require testing
	afterwards, we use a version that will either succeed or call
	fatal().
*/

/* $Id$ */

extern char *Realloc(), *Malloc();

/* reallocation factor */

#define	allocfrac(s)	((s) * 3 / 2)

