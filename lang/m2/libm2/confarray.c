#include <m2_traps.h>

/*	Runtime handling of "value" conformant arrays.
	The routine that accepts the conformant array parameter first calls
	the routine new_stackptr. This routine computes a new stack pointer
	for the calling routine and returns it. The new space on the stack is
	large enough to store the array.
	Then, it calls copy_array to do the copying.
*/

struct descr {
	char *addr;
	int low;
	unsigned int highminlow;
	unsigned int size;
};

static struct descr *descrs[10];
static struct descr **ppdescr = descrs;

char *
_new_stackptr(pdescr, a)
	register struct descr *pdescr;
{
	/*	The parameter "a" is not used and not supplied.
		It's address is the old stack-pointer.
	*/
	unsigned int size = (((pdescr->highminlow + 1) * pdescr->size +
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

_copy_array(p, a)
	register int *p;
{
	register int *q;
	register unsigned int sz;
	char dummy;

	ppdescr--;
	sz = (((*ppdescr)->highminlow + 1) * (*ppdescr)->size) / EM_WSIZE;
	
	if ((char *) &a - (char *) &dummy > 0) {
		(*ppdescr)->addr = (char *) (q = &a);
	}
	else	(*ppdescr)->addr = (char *) (q = &a - sz);

	while (sz--) *q++ = *p++;
}
