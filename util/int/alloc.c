/* $Id$ */

#include	"debug.h"
#include	"global.h"
#include	"alloc.h"

extern char *malloc();
extern char *realloc();

char *Malloc(sz, descr)
	size sz;
	char *descr;
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

char *Realloc(old, sz, descr)
	char *old;
	size sz;
	char *descr;
{
	register char *new = realloc(old, (unsigned int) (sz));
	
	if (new == (char *) 0)
		fatal("Cannot reallocate %s", descr);
	return new;
}

