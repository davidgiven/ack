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

#include	<pc_file.h>

extern int	*_extfl;
extern char	*_hbase;
extern		_wrs();
extern		_wln();

procentry(name) char *name; {
	struct file *f;

	f = EXTFL(2);
	_wrs(5,"call ",f);
	_wrs(8,name,f);
	_wln(f);
}
