/* $Header$ */
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
/*         extracted from hlt.c by C.H. Lindsey */

#include	<pc_file.h>

extern char	*_hbase;
extern int	*_extfl;
extern		_cls();
extern		exit();

_cleanup() {
	int i;

	for (i = 1; i <= _extfl[0]; i++)
		if (_extfl[i] != -1)
			_cls(EXTFL(i));
	return;
}
