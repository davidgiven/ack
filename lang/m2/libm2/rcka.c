/*
 * (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 *
 * Module:	range checks for INTEGER, now for array indexing
 * Author:	Ceriel J.H. Jacobs
 * Version:	$Id$
*/

#include "libm2.h"
#include <em_abs.h>

void rcka(struct array_descr* descr, int indx)
{
	if (indx < 0 || indx > descr->n_elts_min_one)
		TRP(EARRAY);
}
