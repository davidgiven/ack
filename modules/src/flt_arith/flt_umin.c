/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

#include "flt_misc.h"
#if __STDC__ > 0
void
#endif
flt_umin(e)
	flt_arith *e;
{
	/*	Unary minus
	*/
	flt_status = 0;
	e->flt_sign = ! e->flt_sign;
}
