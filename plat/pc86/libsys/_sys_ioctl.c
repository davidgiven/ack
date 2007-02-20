#include <stdlib.h>
#include <errno.h>
#include <sgtty.h>
#include "libsys.h"

int _sys_ttyflags = ECHO;

extern struct
{
	int fd;
	int request;
	void* argp;
} _sys_params_in;

extern struct
{
	int result;
} _sys_params_out;

#define P _sys_params_in

static int tiocgetp(void)
{
	struct sgttyb* s = P.argp;
	s->sg_flags = _sys_ttyflags;
	return 0;
}

static int tiocsetp(void)
{
	struct sgttyb* s = P.argp;
	_sys_ttyflags = s->sg_flags;
	return 0;
}

int _sys_ioctl(void)
{
	switch (P.request)
	{
		case TIOCGETP:
			_sys_params_out.result = tiocgetp();
			return 0;

		case TIOCSETP:
			_sys_params_out.result = tiocsetp();
			return 0;
	}
	
	_sys_params_out.result = -1;
	errno = EINVAL;
	return 0;
}
