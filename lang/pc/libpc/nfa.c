/* $Id$ */
/*
 * (c) copyright 1990 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

/* Author: Hans van Eck */

#include "pc.h"

void _nfa(int bool)
{
	if (!bool)
		_trp(EFUNASS);
}
