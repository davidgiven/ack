/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<signal.h>
#include	<stdlib.h>

void
abort(void)
{
	raise(SIGABRT);
}

