/* $Header$ */
/*	PROGRAM'S INTERFACE TO MEMORY ALLOCATION ROUTINES		*/

/*	This file serves as the interface between the program and the
	memory allocating routines.
	There are 3 memory allocation routines:
		char *Malloc(n)		to allocate n bytes
		char *Salloc(str, n)	to allocate n bytes
						and fill them with string str
		char *Realloc(str, n)	reallocate the string at str to n bytes
*/

extern char *Salloc(), *malloc(), *realloc();

#define	Malloc(n)	malloc((unsigned)(n))
#define	Srealloc(ptr,n)	realloc(ptr, (unsigned)(n))
