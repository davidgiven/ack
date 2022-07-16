/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * (c) copyright 2022 by TK Chia.
 */
/* $Id$ */

#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

void abort(void)
{
	unsigned short count = 0;
	int *bad_ptr = NULL;
	while (--count != 0)
		raise(SIGABRT);
	/*
	 * If the target platform does not implement raise(.), or the
	 * SIGABRT signal turns out to be handled or ignored, then our
	 * process may still be running.  Try harder to make the process
	 * crash or exit.  -- tkchia
	 */
	while (--count != 0)
		abs(*bad_ptr);
	write(2, "abort!\n", 7);
	for (;;)
		_exit(128 + SIGABRT);
}
