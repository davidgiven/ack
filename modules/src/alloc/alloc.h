/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef __ALLOC_INCLUDED__
#define __ALLOC_INCLUDED__
/*	PROGRAM'S INTERFACE TO MEMORY ALLOCATION ROUTINES		*/

/*	This file serves as the interface between the program and the
    memory allocating routines.
    There are 3 memory allocation routines:
        char *malloc(n)		allocate n bytes
*/

char* st_alloc(char**, unsigned int, int);
char* std_alloc(char**, unsigned int, int, int*);

/*	S T R U C T U R E - S T O R A G E  D E F I N I T I O N S	*/

typedef struct _ALLOC_
{
	struct _ALLOC_* _A_next;
}* _PALLOC_;

#define _A_st_free(ptr, phead, size)                                                               \
	(((_PALLOC_)ptr)->_A_next = (_PALLOC_)(*phead), *((_PALLOC_*)phead) = (_PALLOC_)ptr)
#define st_free(ptr, phead, size) _A_st_free(ptr, phead, size)

#endif /* __ALLOC_INCLUDED__ */
