/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*	PROGRAM'S INTERFACE TO MEMORY ALLOCATION ROUTINES		*/

/*	This file serves as the interface between the program and the
	memory allocating routines.
	There are 3 memory allocation routines:
		char *Malloc(n)		allocate n bytes
		char *Salloc(str, n)	allocate n bytes and fill them with
					string str
		char *Realloc(str, n)	reallocate the block at str to n bytes.
		char *Srealloc(str, n)	same as Realloc.
*/

extern char *Salloc(), *Malloc(), *Srealloc(), *Realloc();
extern char *malloc(), *realloc();

/*	S T R U C T U R E - S T O R A G E  D E F I N I T I O N S	*/

#ifndef	BOTCH_FREE
#define	st_free(ptr, phead, size)	(ptr->next = *phead, *phead = ptr)
#else	def BOTCH_FREE
#define	st_free(ptr, phead, size)	(botch((char *)(ptr), size), \
						ptr->next = *phead, *phead = ptr)
#endif	BOTCH_FREE
