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

#include	<pc_err.h>

extern		_trp();

#define	assert()	/* nothing */

struct descr {
	int	low;
	int	diff;
	int	size;
};

_unp(ad,zd,i,ap,zp) int i; struct descr *ad,*zd; char *ap,*zp; {

	if (zd->diff > ad->diff ||
			(i -= ad->low) < 0 ||
			(i+zd->diff) > ad->diff)
		_trp(EUNPACK);
	ap += (i * ad->size);
	i = (zd->diff + 1) * zd->size;
	if (zd->size == 1) {
		assert(ad->size == 2);
		while (--i >= 0)
			*((int *)ap)++ = *zp++;
	} else {
		assert(ad->size == zd->size);
		while (--i >= 0)
			*ap++ = *zp++;
	}
}
