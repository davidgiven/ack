/* $Header$ */
/*	M E M O R Y  A L L O C A T I O N  R O U T I N E S	*/

/*	The allocation of memory in this program, which plays an important
	role in reading files, replacing macros and building expression
	trees, is not performed by malloc etc.  The reason for having own
	memory allocation routines (malloc(), realloc() and free()) is
	plain: the garbage collection performed by the library functions
	malloc(), realloc() and free() costs a lot of time, while in most
	cases (on a VAX) the freeing and reallocation of memory is not
	necessary.  The only reallocation done in this program is at
	building strings in memory.  This means that the last
	(re-)allocated piece of memory can be extended.

	The (basic) memory allocating routines offered by this memory
	handling package are:

	char *malloc(n)		: allocate n bytes
	char *realloc(ptr, n)	: reallocate buffer to n bytes
					(works only if ptr was last allocated)
	free(ptr)		: if ptr points to last allocated
					memory, this memory is re-allocatable
	Salloc(str, sz)		: save string in malloc storage
*/

#include	<system.h>
#include	"myalloc.h"	/* UF */
#include	"debug.h"	/* UF */

#include	"alloc.h"
#include	"assert.h"

#ifdef	OWNALLOC
char *sys_break();
/* the following variables are used for book-keeping		 */
static int nfreebytes = 0;	/* # free bytes in sys_break space */
static char *freeb;		/* pointer to first free byte	 */
static char *lastalloc;		/* pointer to last malloced sp	 */
static int lastnbytes;		/* nr of bytes in last allocated */
				/* space			 */
static char *firstfreeb = 0;

#endif	OWNALLOC

char *
Salloc(str, sz)
	register char str[];
	register int sz;
{
	/*	Salloc() is not a primitive function: it just allocates a
		piece of storage and copies a given string into it.
	*/
	char *res = Malloc(sz);
	register char *m = res;

	while (sz--)
		*m++ = *str++;
	return res;
}

#ifdef	OWNALLOC

#define	ALIGN(m)	(ALIGNSIZE * (((m) - 1) / ALIGNSIZE + 1))

char *
malloc(n)
	unsigned n;
{
	/*	malloc() is a very simple malloc().
	*/
	n = ALIGN(n);
	if (nfreebytes < n)	{
		register nbts = (n <= ALLOCSIZ) ? ALLOCSIZ : n;

		if (!nfreebytes)	{
			if ((freeb = sys_break(nbts)) == ILL_BREAK)
				fatal("out of memory");
		}
		else	{
			if (sys_break(nbts) == ILL_BREAK)
				fatal("out of memory");
		}
		nfreebytes += nbts;
	}
	lastalloc = freeb;
	freeb = lastalloc + n;
	lastnbytes = n;
	nfreebytes -= n;
	return lastalloc;
}

/*ARGSUSED*/
char *
realloc(ptr, n)
	char *ptr;
	unsigned n;
{
	/*	realloc() is designed to append more bytes to the latest
		allocated piece of memory. However reallocation should be
		performed, even if the mentioned memory is not the latest
		allocated one, this situation will not occur. To do so,
		realloc should know how many bytes are allocated the last
		time for that piece of memory. ????
	*/
	register int nbytes = n;

	ASSERT(ptr == lastalloc);	/* security		*/
	nbytes -= lastnbytes;		/* # bytes required	*/
	if (nbytes == 0)		/* no extra bytes	*/
		return lastalloc;

	/*	if nbytes < 0: free last allocated bytes;
		if nbytes > 0: allocate more bytes
	*/
	if (nbytes > 0)
		nbytes = ALIGN(nbytes);
	if (nfreebytes < nbytes)	{
		register int nbts = (nbytes < ALLOCSIZ) ? ALLOCSIZ : nbytes;
		if (sys_break(nbts) == ILL_BREAK)
			fatal("out of memory");
		nfreebytes += nbts;
	}
	freeb += nbytes;	/* less bytes			*/
	lastnbytes += nbytes;	/* change nr of last all. bytes	*/
	nfreebytes -= nbytes;	/* less or more free bytes	*/
	return lastalloc;
}

/* to ensure that the alloc library package will not be loaded:	*/
/*ARGSUSED*/
free(p)
	char *p;
{}

init_mem()
{
	firstfreeb = sys_break(0);
	/* align the first memory unit to ALIGNSIZE ???	*/
	if ((long) firstfreeb % ALIGNSIZE != 0) {
		register char *fb = firstfreeb;

		fb = (char *)ALIGN((long)fb);
		firstfreeb = sys_break(fb - firstfreeb);
		firstfreeb = fb;
		ASSERT((long)firstfreeb % ALIGNSIZE == 0);
	}
}

#ifdef	DEBUG
mem_stat()
{
	extern char options[];

	if (options['m'])
		printf("Total nr of bytes allocated: %d\n",
			sys_break(0) - firstfreeb);
}
#endif	DEBUG
#endif	OWNALLOC
