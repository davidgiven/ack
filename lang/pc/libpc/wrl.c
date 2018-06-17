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

#include "pc.h"
#include <limits.h>

void _wsl(int w, long l, struct file* f)
{
	char *p, c;
	long j;
	char buf[11];

	if (w < 0)
		_trp(EWIDTH);
	p = &buf[11];
	if ((j = l) < 0)
	{
		if (l == LONG_MIN)
		{
			_wstrin(w, 11, "-2147483648", f);
			return;
		}
		j = -j;
	}
	do
	{
		c = j % 10;
		*--p = c + '0';
	} while (j /= 10);
	if (l < 0)
		*--p = '-';
	_wstrin(w, (int)(&buf[11] - p), p, f);
}

void _wrl(long l, struct file* f)
{
	_wsl(11, l, f);
}
