/* $Header$ */
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
		char *Malloc(n)		allocate n bytes
		char *Salloc(str, n)	allocate n bytes and fill them with
					string str
		char *Realloc(str, n)	reallocate the block at str to n bytes.
		char *Srealloc(str, n)	same as Realloc.
*/

#if __STDC__
char *Malloc(unsigned int);
char *Salloc(char *, unsigned int);
char *Srealloc(char *, unsigned int);
char *Realloc(char *, unsigned int);
char *st_alloc(char **, unsigned int, int);
char *std_alloc(char **, unsigned int, int, int *);
void No_Mem(void);
void clear(char *, unsigned int);
void botch(char *, unsigned int);
#else
extern char	*Salloc(), *Malloc(), *Srealloc(), *Realloc();
extern char	*st_alloc(), *std_alloc();
void		clear(), botch(), No_Mem();
#endif

/*	S T R U C T U R E - S T O R A G E  D E F I N I T I O N S	*/

typedef struct _ALLOC_ {
	struct _ALLOC_ *_A_next;
} *_PALLOC_;


#define	_A_st_free(ptr, phead, size)	(((_PALLOC_)ptr)->_A_next = \
						(_PALLOC_)(*phead), \
					 *((_PALLOC_ *)phead) = \
						(_PALLOC_) ptr)
#ifndef	BOTCH_FREE
#define st_free(ptr, phead, size)	_A_st_free(ptr, phead, size)
#else	/* def BOTCH_FREE */
#define	st_free(ptr, phead, size)	(botch((char *)(ptr), size), \
						_A_st_free(ptr, phead, size))
#endif	/* BOTCH_FREE */

#define Free(p)	free(p)

#endif /* __ALLOC_INCLUDED__ */
