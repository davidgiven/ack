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
	unsigned int size = (((pdescr->highminlow + 1) * pdescr->size +
				(EM_WSIZE - 1)) & ~(EM_WSIZE - 1));

	if (ppdescr >= &descrs[10]) {
		/* to many nested traps + handlers ! */
		TRP(65);
	}
	*ppdescr++ = pdescr;
	if ((char *) &a - (char *) &size > 0) {
		/* stack grows downwards */
		return (char *) &a - size;
	}
	else	return (char *) &a + size;
}

_copy_array(p, a)
	register char *p;
{
	register char *q;
	register unsigned int sz;
	char dummy;

	ppdescr--;
	sz = (((*ppdescr)->highminlow + 1) * (*ppdescr)->size +
		(EM_WSIZE -1)) & ~ (EM_WSIZE - 1);
	
	if ((char *) &a - (char *) &dummy > 0) {
		(*ppdescr)->addr = q = (char *) &a;
	}
	else	(*ppdescr)->addr = q = (char *) &a - sz;

	while (sz--) *q++ = *p++;
}
