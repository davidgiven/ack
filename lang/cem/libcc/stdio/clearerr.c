/* $Id$ */

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#include <stdio.h>

clearerr(iop)
	FILE *iop;
{
	iop->_flags &= ~(IO_ERR|IO_EOF);
}
