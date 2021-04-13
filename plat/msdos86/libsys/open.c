/* $Source$
 * $State$
 * $Revision$
 */

/*
 * Derived from dos-open.c for newlib-ia16 which was written for the
 * gcc-ia16 toolchain.
 *
 * Copyright (c) 2018 Bart Oldeman
 * Copyright (c) 2019--2021 TK Chia
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include "libsys.h"

int open(const char* pathname, int flags, ...)
{
	int fd = -1, binmode;
	off_t ret;

	if ((flags & ~(O_ACCMODE | O_CREAT | O_TRUNC | O_APPEND
		       | O_TEXT | O_BINARY)))
	{
		/* bail out if there are unknown flags */
		errno = EINVAL;
		return -1;
	}

	binmode = flags & (O_TEXT | O_BINARY);
	if (binmode != O_TEXT && binmode != O_BINARY && binmode != 0)
	{
		/* bail out if both O_TEXT and O_BINARY are specified (!) */
		errno = EINVAL;
		return -1;
	}

	if ((flags & O_CREAT))
	{
		/* special but common case O_WRONLY | O_CREAT | O_TRUNC
		 * should be handled by only calling _sys_rawcreat */
		int fileexists = 0;
		if (!(flags & O_TRUNC))
			fileexists = _sys_exists(pathname);
		if (!fileexists)
		{
			va_list ap;
			mode_t mode;

			va_start(ap, flags);
			mode = va_arg(ap, mode_t);
			va_end(ap);

			fd = _sys_rawcreat(pathname, mode & 0200 ? 0 : 1);
			if (fd != -1)
			{
				if ((flags & O_ACCMODE) == O_WRONLY)
					return fd;
				close(fd);
				fd = -1;
			}
		}
	}

	/* try to open file with mode */
	if (fd == -1)
		fd = _sys_rawopen(pathname, flags & O_ACCMODE);
	if (fd == -1)
		return fd;
	ret = 0;

	/* handle O_TRUNC and O_APPEND */
	if ((flags & O_TRUNC))
		ret = _sys_rawwrite(fd, NULL, 0);
	else if ((flags & O_APPEND))
		ret = _sys_rawlseek(fd, 0L, SEEK_END);
	if (ret == -1)
	{
		close(fd);
		return -1;
	}

	/* handle O_TEXT and O_BINARY, and reset "end of file encountered" */
	_sys_setmode(fd, binmode == O_BINARY ? O_BINARY : O_TEXT);
	_sys_seteof(fd, 0);

	return fd;
}
