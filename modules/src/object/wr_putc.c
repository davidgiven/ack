/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "obj.h"

extern int __sectionnr;
void __wr_flush(struct fil *);

void
wr_putc(int ch)
{
	struct fil *ptr = &__parts[PARTEMIT+getsect(__sectionnr)];

	if (ptr->cnt == 0) __wr_flush(ptr);
	ptr->cnt--; *ptr->pnow++ = ch;
}
