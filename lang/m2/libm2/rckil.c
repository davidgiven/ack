/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 *
 * Module:	range checks for LONGINT
 * Author:	Ceriel J.H. Jacobs
 * Version:	$Id$
*/

#include "libm2.h"
#include <em_abs.h>

void rckil(struct long_range_descr* descr, long val)
{
	if (val < descr->low || val > descr->high)
		TRP(ERANGE);
}
