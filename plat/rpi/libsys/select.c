/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>
#include <pi.h>
#include "libsys.h"

#define TICKS_PER_SEC 1000000

typedef int condition_t(void);

static int nop_condition(void)
{
	return 0;
}

int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout)
{
	int result = 0;
	condition_t* condition = nop_condition;

	if (FD_ISSET(0, readfds))
		condition = _sys_rawpoll;

	FD_ZERO(readfds);
	FD_ZERO(writefds);
	FD_ZERO(exceptfds);

	if (timeout)
	{
		/* Wait for a specified amount of time. */

		uint32_t ticks = (timeout->tv_sec * TICKS_PER_SEC) +
			(timeout->tv_usec * (TICKS_PER_SEC/1000000));
		uint32_t* timer_clo = pi_phys_to_user((void*) 0x7e003004);
		uint32_t ra = *timer_clo;

		while (!condition() && ((*timer_clo - ra) < ticks))
			;
	}
	else
	{
		/* Wait forever. */

		while (!condition())
			;

	}

	if ((condition == _sys_rawpoll) && condition())
	{
		FD_SET(0, readfds);
		result = 1;
	}

	return result;
}
