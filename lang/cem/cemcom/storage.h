/* $Header$ */
/*	S T R U C T U R E - S T O R A G E  D E F I N I T I O N S	*/

/*	Storage allocation is one of the most expensive operations in
	the compiler and consequently much thought and experimentation
	has gone into it.  To simplify the hooking in of new super-fancy
	algorithms, all allocating and freeing of storage for structs
	goes through the macros
		st_alloc(&head, size)
		st_free(ptr, head, size)
	which, hopefully, convey enough information.
*/

extern char *head_alloc();

#define	st_alloc(headp, size)		head_alloc((char **)headp, size)

#ifndef	BOTCH_FREE
#define	st_free(ptr, head, size)	(ptr->next = head, head = ptr)
#else	def BOTCH_FREE
#define	st_free(ptr, head, size)	(botch((char *)(ptr), size), \
						ptr->next = head, head = ptr)
#endif	BOTCH_FREE
