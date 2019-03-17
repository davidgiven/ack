/*
	Rather than using malloc and realloc, which require testing
	afterwards, we use a version that will either succeed or call
	fatal().
*/
#ifndef ALLOC_H_
#define ALLOC_H_

#include	"global.h"

char *Malloc(size sz, char *descr);
char *Realloc(char *old, size sz, char *descr);


/* reallocation factor */

#define	allocfrac(s)	((s) * 3 / 2)

#endif /* ALLOC_H_ */
