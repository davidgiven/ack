/* $Header$ */
/*	S T R U C T U R E - S T O R A G E  M A N A G E M E N T		*/

/*	Assume that each structure contains a field "next", of pointer
	type, as first tagfield.
	struct xxx serves as a general structure: it just declares the
	tagfield "next" as first field of a structure.
	Please don't worry about any warnings when compiling this file
	because some dirty tricks are performed to obtain the necessary
	actions.
*/

#include	"debug.h"	/* UF */
#include	"botch_free.h"	/* UF */
#include	"assert.h"
#include	"alloc.h"
#include	"storage.h"

struct xxx	{
	char *next;
};

char *
st_alloc(phead, size)
	char **phead;
	int size;
{
	struct xxx *tmp;

	if (*phead == 0)	{
		return Malloc(size);
	}
	tmp = (struct xxx *) (*phead);
	*phead = (char *) tmp->next;
	return (char *) tmp;
}

/* instead of Calloc:	*/
clear(ptr, n)
	char *ptr;
	int n;
{
	ASSERT((long)ptr % sizeof (long) == 0);
	while (n >= sizeof (long))	{	/* high-speed clear loop */
		*(long *)ptr = 0L;
		ptr += sizeof (long), n -= sizeof (long);
	}
	while (n--)
		*ptr++ = '\0';
}

#ifdef	BOTCH_FREE
botch(ptr, n)
	char *ptr;
	int n;
{	/*	Writes garbage over n chars starting from ptr.
		Used to check if freed memory is used inappropriately.
	*/
	ASSERT((long)ptr % sizeof (long) == 0);
	while (n >= sizeof (long))	{	/* high-speed botch loop */
		*(long *)ptr = 025252525252L;
		ptr += sizeof (long), n -= sizeof (long);
	}
	while (n--)
		*ptr++ = '\252';
}
#endif	BOTCH_FREE
