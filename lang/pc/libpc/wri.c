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

#include	<pc_file.h>

extern		_wstrin();

_wsi(w,i,f) int w,i; struct file *f; {
	char *p; int j; char buf[6];

	p = &buf[6];
	if ((j=i) < 0) {
		if (i == -32768) {
			_wstrin(w,6,"-32768",f);
			return;
		}
		j = -j;
	}
	do
		*--p = '0' + j%10;
	while (j /= 10);
	if (i<0)
		*--p = '-';
	_wstrin(w,&buf[6]-p,p,f);
}

_wri(i,f) int i; struct file *f; {
	_wsi(6,i,f);
}
