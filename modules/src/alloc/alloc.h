/*	PROGRAM'S INTERFACE TO MEMORY ALLOCATION ROUTINES		*/

/*	This file serves as the interface between the program and the
	memory allocating routines.
	There are 3 memory allocation routines:
		char *Malloc(n)		allocate n bytes
		char *Salloc(str, n)	allocate n bytes and fill them with
					string str
		char *Realloc(str, n)	reallocate the string at str to n bytes,
					only works if str was last allocated
*/

extern char *Salloc(), *Malloc(), *Srealloc();
extern char *malloc(), *realloc();

/*	S T R U C T U R E - S T O R A G E  D E F I N I T I O N S	*/

#ifndef	BOTCH_FREE
#define	st_free(ptr, phead, size)	(ptr->next = *phead, *phead = ptr)
#else	def BOTCH_FREE
#define	st_free(ptr, phead, size)	(botch((char *)(ptr), size), \
						ptr->next = *phead, *phead = ptr)
#endif	BOTCH_FREE
