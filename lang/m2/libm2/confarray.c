/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/*
  Module:	runtime support for conformant arrays
  Author:	Ceriel J.H. Jacobs
  Version:	$Id$
*/
#include "libm2.h"
#include <m2_traps.h>

#ifndef EM_WSIZE
#define EM_WSIZE _EM_WSIZE
#define EM_PSIZE _EM_PSIZE
#endif

static struct stack_descr* descrs[10];
static struct stack_descr** ppdescr = descrs;

size_t new_stackptr(struct stack_descr* pdescr, int a)
{
	size_t size = (((pdescr->highminlow + 1) * pdescr->size + (EM_WSIZE - 1)) & ~(EM_WSIZE - 1));

	if (ppdescr >= &descrs[10])
	{
		/* to many nested traps + handlers ! */
		TRP(M2_TOOMANY);
	}
	*ppdescr++ = pdescr;
	if ((char*)&a - (char*)&pdescr > 0)
	{
		/* stack grows downwards */
		return -size;
	}
	return size;
}

void copy_array(char* p, int a)
{
	char* q;
	size_t sz;
	char dummy;

	ppdescr--;
	sz = ((*ppdescr)->highminlow + 1) * (*ppdescr)->size;

	if ((char*)&a - (char*)&p > 0)
	{
		(*ppdescr)->addr = q = (char*)&a;
	}
	else
		(*ppdescr)->addr = q = (char*)&a - ((sz + (EM_WSIZE - 1)) & ~(EM_WSIZE - 1));

	while (sz--)
		*q++ = *p++;
}
