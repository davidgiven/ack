/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	runtime support for conformant arrays
  Author:	Ceriel J.H. Jacobs
  Version:	$Header$
*/
#include <m2_traps.h>

#if EM_WSIZE==EM_PSIZE
typedef unsigned pcnt;
#else
typedef unsigned long pcnt;
#endif

struct descr {
	char *addr;
	int low;
	unsigned int highminlow;
	unsigned int size;
};

static struct descr *descrs[10];
static struct descr **ppdescr = descrs;

char *
new_stackptr(pdescr, a)
	register struct descr *pdescr;
{
	pcnt size = (((pdescr->highminlow + 1) * pdescr->size +
				(EM_WSIZE - 1)) & ~(EM_WSIZE - 1));

	if (ppdescr >= &descrs[10]) {
		/* to many nested traps + handlers ! */
		TRP(M2_TOOMANY);
	}
	*ppdescr++ = pdescr;
	if ((char *) &a - (char *) &size > 0) {
		/* stack grows downwards */
		return (char *) &a - size;
	}
	else	return (char *) &a + size;
}

copy_array(p, a)
	register char *p;
{
	register char *q;
	register pcnt sz;
	char dummy;

	ppdescr--;
	sz = ((*ppdescr)->highminlow + 1) * (*ppdescr)->size;
	
	if ((char *) &a - (char *) &dummy > 0) {
		(*ppdescr)->addr = q = (char *) &a;
	}
	else	(*ppdescr)->addr = q = (char *) &a - 
			((sz + (EM_WSIZE - 1)) & ~ (EM_WSIZE - 1));

	while (sz--) *q++ = *p++;
}
