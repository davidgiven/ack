/*	M E M O R Y  A L L O C A T I O N  R O U T I N E S	*/

/*	The reason for having own memory allocation routines (malloc(),
	realloc() and free()) is plain: the garbage collection performed by
	the library functions malloc(), realloc() and free() costs a lot of
	time, while in most cases (on a VAX) the freeing and reallocation of
	memory is not necessary.
	The (basic) memory allocating routines offered by this memory
	handling package are:

	char *malloc(n)		: allocate n bytes
	char *realloc(ptr, n)	: reallocate buffer to n bytes
					(works only if ptr was last allocated)
	free(ptr)		: if ptr points to last allocated
					memory, this memory is re-allocatable

	This module imports routines from "system", an assertion macro,
	and the compile-time
	constants ALIGNBITS, ALLOCSIZ, DEBUG.
	ALIGNBITS is an integer constant defining suitable alignment,
	ALLOCSIZ is the size of the chunks of memory asked from the system,
	DEBUG enables the assertions.
*/

#include	<assert.h>
#include	<system.h>

#ifndef ALIGNBITS
#define ALIGNBITS	07
#endif

#ifndef ALLOCSIZ
#define ALLOCSIZ	4096
#endif

/* the following variables are used for book-keeping */
static int nfreebytes = 0;	/* # free bytes in sys_break-ed space */
static char *freeb = 0;		/* pointer to first free byte */
static char *lastalloc;		/* pointer to last malloced sp */
static int lastnbytes;		/* nr of bytes in last allocated space */
static char *firstfreeb;	/* pointer to first ever free byte */

#define	ALIGN(m) (((m)&ALIGNBITS)? (m)+((1+ALIGNBITS)-((m)&ALIGNBITS)):(m))

char *sys_break();

char *
malloc(n)
	unsigned int n;
{
	/*	malloc() is a very simple malloc().
	*/

	n = ALIGN(n);
	if (nfreebytes < n)	{
		register int nbts = (n <= ALLOCSIZ) ? ALLOCSIZ : n;

		if (!nfreebytes)	{
			if (!firstfreeb) {
				/* We arrive here the first time malloc is
				   called
				*/
				int diff;

				if (!(freeb = sys_break(0))) return 0;
				if ((diff = (int)((long)freeb&ALIGNBITS))!=0) {
					/* align memory to ALIGNBITS ... */
					diff = (1 + ALIGNBITS) - diff;
					if (!(freeb = sys_break(diff))) {
					      return 0;
					}
					freeb += diff;
					assert(((long)freeb & ALIGNBITS) == 0);
				}
				firstfreeb = freeb;
			}
			if (!(freeb = sys_break(nbts))) return 0;
		}
		else	{
			if (!sys_break(nbts)) return 0;
		}
		nfreebytes += nbts;
	}
	lastalloc = freeb;
	freeb = lastalloc + n;
	lastnbytes = n;
	nfreebytes -= n;
	return lastalloc;
}

char *
realloc(ptr, n)
	char *ptr;
	unsigned int n;
{
	/*	realloc() is designed to append more bytes to the latest
		allocated piece of memory.
	*/
	register int nbytes = n;

	if (!ptr || ptr != lastalloc) {	/* security */
		return 0;
	}
	nbytes -= lastnbytes;		/* # bytes required */
	if (nbytes == 0)	{	/* no extra bytes */
		return lastalloc;
	}

	/*	if nbytes < 0: free last allocated bytes;
		if nbytes > 0: allocate more bytes
	*/
	if (nbytes > 0) nbytes = ALIGN(nbytes);
	if (nfreebytes < nbytes)	{
		register int nbts = (nbytes < ALLOCSIZ) ? ALLOCSIZ : nbytes;
		if (!sys_break(nbts)) return 0;
		nfreebytes += nbts;
	}
	freeb += nbytes;	/* less bytes */
	lastnbytes += nbytes;	/* change nr of last all. bytes */
	nfreebytes -= nbytes;	/* less or more free bytes */
	return lastalloc;
}

free(p)
	char *p;
{
	if (lastalloc && lastalloc == p) {
		nfreebytes += lastnbytes;
		freeb = lastalloc;
		lastnbytes = 0;
		lastalloc = 0;
	}
}

#ifdef	DEBUG
mem_stat()
{

	printf("Total nr of bytes allocated: %d\n",
		sys_break(0) - firstfreeb);
}
#endif	DEBUG
