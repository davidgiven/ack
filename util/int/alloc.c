/** @file
 *  Memory allocation routines that will cause
 *  fatal error if allocation fails.
 */
#include	<stdlib.h>
#include	"debug.h"
#include	"global.h"
#include	"alloc.h"
#include	"io.h"

/** Allocate "sz" bytes on the heap with description
 * "descr", raise a fatal error if it cannot be
 * allocated. Returns a pointer to the newly allocated
 * block.
 */
char *Malloc(size sz, char *descr)
{
	register char *new = malloc((unsigned int) (sz));
	
	if (new == (char *) 0 && descr != (char *) 0)
		fatal("Cannot allocate %s", descr);

#ifdef	DB_MALLOC			/* from debug.h */
	/* fill area with recognizable garbage */
	{	register char *p = new;
		register size i = sz;
		register char ch = 0252;

		if (p) {
			while (i--) {
				*p++ = ch;
				ch = ~ch;
			}
		}
	}
#endif	/* DB_MALLOC */

	return new;
}

/** Reallocates an "old" memory block with new size
 * "sz" in bytes. Raise a fatal error if the block
 * cannot be reallocated.
 *
 */
char *Realloc(char *old, size sz, char *descr)
{
	register char *new = realloc(old, (unsigned int) (sz));
	
	if (new == (char *) 0)
		fatal("Cannot reallocate %s", descr);
	return new;
}

