/* $Id$ */
/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* Author: Hans van Eck */

#include "pc.h"
#include <em_abs.h>

void _rcka(struct array_descr* descr, int index)
{
	if (index < descr->lbound || index > (int)descr->n_elts_min_one + descr->lbound)
		_trp(EARRAY);
}
