/* $Header$ */
/*	S T R U C T U R E - S T O R A G E  D E F I N I T I O N S	*/

#ifndef	BOTCH_FREE
#define	st_free(ptr, head, size)	{ptr->next = head; head = ptr;}
#else	def BOTCH_FREE
#define	st_free(ptr, head, size)	{botch((char *)(ptr), size); \
						ptr->next = head; head = ptr;}
#endif	BOTCH_FREE
