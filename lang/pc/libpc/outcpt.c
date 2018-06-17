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
#include <unistd.h>
#include <pc_file.h>
#include <pc_err.h>

extern _trp();

_flush(f) struct file* f;
{
	int i, n;

	f->ptr = f->bufadr;
	n = f->buflen - f->count;
	if (n <= 0)
		return;
	f->count = f->buflen;
	if ((i = write(f->ufd, f->bufadr, n)) < 0 && errno == EINTR)
		return;
	if (i != n)
		_trp(EWRITE);
}

_outcpt(f) struct file* f;
{

	f->flags &= ~ELNBIT;
	f->ptr += f->size;
	if ((f->count -= f->size) <= 0)
		_flush(f);
}
