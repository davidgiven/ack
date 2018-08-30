/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

#if ACKCONF_WANT_EMULATED_RAISE

int raise(int sig)
{
	if (sig < 0 || sig > _NSIG)
		return -1;
	return kill(getpid(), sig);
}

#endif
