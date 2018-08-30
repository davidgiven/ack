/* $Id$ */
/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/* Author: J.W. Stevenson */

#include <stdlib.h>
#include <errno.h>
#include "pc.h"

int _efl(struct file* f)
{

	_curfil = f;
	if ((f->flags & 0377) != MAGIC)
		_trp(EBADF);
	if ((f->flags & (WINDOW | WRBIT | EOFBIT)) == 0)
		_incpt(f);
	return ((f->flags & EOFBIT) != 0);
}
